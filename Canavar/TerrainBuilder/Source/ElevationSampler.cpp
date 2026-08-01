#include "ElevationSampler.h"
#include "GeoTiffReader.h"

#include <QDir>
#include <QRegularExpression>
#include <cmath>

namespace Canavar::Globe
{
    ElevationSampler::ElevationSampler(const QString& DtedFolder, int CacheSize)
        : mDtedFolder(DtedFolder), mMaxCache(CacheSize)
    {}

    void ElevationSampler::ScanDtedFolder()
    {
        mAvailableFiles.clear();
        const QDir Dir(mDtedFolder);

        // Expected name pattern: Copernicus_DSM_COG_10_N41_00_E028_00_DEM.tif
        static const QRegularExpression Re(
            R"(Copernicus_DSM_COG_\d+_([NS])(\d+)_\d+_([EW])(\d+)_\d+_DEM\.tif)",
            QRegularExpression::CaseInsensitiveOption);

        for (const QFileInfo& Fi :
             Dir.entryInfoList(QStringList() << "*.tif" << "*.tiff",
                               QDir::Files | QDir::NoDotAndDotDot))
        {
            const auto Match = Re.match(Fi.fileName());
            if (!Match.hasMatch()) continue;

            const int Lat = Match.captured(2).toInt() * (Match.captured(1) == "S" ? -1 : 1);
            const int Lon = Match.captured(4).toInt() * (Match.captured(3) == "W" ? -1 : 1);
            mAvailableFiles.insert({ Lat, Lon }, Fi.absoluteFilePath());
        }
    }

    bool ElevationSampler::HasTile(int LatFloor, int LonFloor) const
    {
        return mAvailableFiles.contains({ LatFloor, LonFloor });
    }

    const ElevationSampler::ElevGrid* ElevationSampler::FindCached(int LatFloor, int LonFloor) const
    {
        for (const auto& G : mCache)
            if (G.LatFloor == LatFloor && G.LonFloor == LonFloor)
                return &G;
        return nullptr;
    }

    bool ElevationSampler::LoadTile(int LatFloor, int LonFloor)
    {
        const auto Key = QPair<int,int>(LatFloor, LonFloor);
        if (!mAvailableFiles.contains(Key)) return false;

        GeoTiffReader Reader;
        if (!Reader.Open(mAvailableFiles[Key])) return false;

        ElevGrid Grid;
        Grid.LatFloor = LatFloor;
        Grid.LonFloor = LonFloor;

        if (!Reader.ReadAll(Grid.Data)) return false;

        Grid.Width  = Reader.Width();
        Grid.Height = Reader.Height();

        // Evict LRU entry if cache is full
        if (mCache.size() >= mMaxCache)
            mCache.removeLast();

        mCache.prepend(std::move(Grid));
        return true;
    }

    double ElevationSampler::SampleGrid(const ElevGrid& Grid, double LatDeg, double LonDeg) const
    {
        // Row 0 is the NORTH edge (LatFloor + 1), row (H-1) is the SOUTH edge (LatFloor).
        // Col 0 is the WEST edge (LonFloor), col (W-1) is the EAST edge (LonFloor + 1).
        const double NorthEdge = static_cast<double>(Grid.LatFloor + 1);
        const double WestEdge  = static_cast<double>(Grid.LonFloor);
        const double RowScale  = static_cast<double>(Grid.Height - 1u);
        const double ColScale  = static_cast<double>(Grid.Width  - 1u);

        const double R = (NorthEdge - LatDeg)  * RowScale;
        const double C = (LonDeg - WestEdge)   * ColScale;

        const int R0 = static_cast<int>(R);
        const int C0 = static_cast<int>(C);
        const int R1 = R0 + 1;
        const int C1 = C0 + 1;

        const double Tr = R - R0;
        const double Tc = C - C0;

        const int W = static_cast<int>(Grid.Width);
        const int H = static_cast<int>(Grid.Height);

        auto Clamp = [](int V, int Lo, int Hi) { return V < Lo ? Lo : (V > Hi ? Hi : V); };
        const double E00 = Grid.Data[Clamp(R0, 0, H-1) * W + Clamp(C0, 0, W-1)];
        const double E10 = Grid.Data[Clamp(R1, 0, H-1) * W + Clamp(C0, 0, W-1)];
        const double E01 = Grid.Data[Clamp(R0, 0, H-1) * W + Clamp(C1, 0, W-1)];
        const double E11 = Grid.Data[Clamp(R1, 0, H-1) * W + Clamp(C1, 0, W-1)];

        // Clamp no-data value (-32768)
        auto Nd = [](double V) { return (V < -9990.0) ? 0.0 : V; };
        return (1.0 - Tr) * ((1.0 - Tc) * Nd(E00) + Tc * Nd(E01))
             +        Tr  * ((1.0 - Tc) * Nd(E10) + Tc * Nd(E11));
    }

    double ElevationSampler::SampleElevation(double LatDeg, double LonDeg)
    {
        const int LatFloor = static_cast<int>(std::floor(LatDeg));
        const int LonFloor = static_cast<int>(std::floor(LonDeg));

        const ElevGrid* Grid = FindCached(LatFloor, LonFloor);
        if (!Grid)
        {
            if (!LoadTile(LatFloor, LonFloor)) return 0.0;
            Grid = &mCache.front();
        }
        else
        {
            // Move to front (LRU)
            const int Idx = static_cast<int>(Grid - mCache.constData());
            if (Idx != 0) mCache.move(Idx, 0);
            Grid = &mCache.front();
        }

        return SampleGrid(*Grid, LatDeg, LonDeg);
    }

} // namespace Canavar::Globe
