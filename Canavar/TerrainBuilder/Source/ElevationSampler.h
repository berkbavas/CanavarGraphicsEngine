#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  ElevationSampler
//  Bilinearly samples elevation at arbitrary (lat, lon) from one or more
//  loaded GLO-30 TIFF tiles.  Tiles are lazy-loaded with a small LRU cache.
// ─────────────────────────────────────────────────────────────────────────────

#include <QHash>
#include <QPair>
#include <QVector>
#include <QString>
#include <QList>

namespace Canavar::Globe
{
    class ElevationSampler
    {
      public:
        explicit ElevationSampler(const QString& DtedFolder, int CacheSize = 8);

        // Register all GLO-30 .tif files found in DtedFolder.
        void ScanDtedFolder();

        // True if a DTED tile covering (LatFloor, LonFloor) is available.
        bool HasTile(int LatFloor, int LonFloor) const;

        // Bilinear sample at arbitrary (lat, lon). Returns the elevation in metres,
        // or 0.0 if no data is available for that location.
        double SampleElevation(double LatDeg, double LonDeg);

      private:
        // A single loaded DTED grid (3601 × 3601 INT16).
        struct ElevGrid
        {
            int LatFloor{ 0 };  // SW corner latitude  (integer degrees)
            int LonFloor{ 0 };  // SW corner longitude (integer degrees)
            uint32_t Width{ 0 }, Height{ 0 };
            QVector<qint16> Data; // row-major, top-left = (LatFloor+1, LonFloor)
        };

        bool LoadTile(int LatFloor, int LonFloor);
        const ElevGrid* FindCached(int LatFloor, int LonFloor) const;

        // Sample from a grid without any bounds check.
        double SampleGrid(const ElevGrid& Grid, double LatDeg, double LonDeg) const;

        QString mDtedFolder;
        QHash<QPair<int,int>, QString> mAvailableFiles; // (lat,lon) → path
        QList<ElevGrid>                mCache;          // LRU: front = most recent
        int                            mMaxCache;
    };

} // namespace Canavar::Globe
