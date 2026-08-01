#include "TexturePacker.h"

#include <QDir>
#include <QPainter>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Canavar::Globe
{
    TexturePacker::TexturePacker(const QString& AlbedoDir, int ZoomLevel, int OutputSize)
        : mAlbedoDir(AlbedoDir), mZoom(ZoomLevel), mOutputSize(OutputSize)
    {}

    void TexturePacker::LatLonToTileXY(double LatDeg, double LonDeg, int Z,
                                        int& TileX, int& TileY,
                                        double& PixelX, double& PixelY)
    {
        const int    N       = 1 << Z;
        const double LatRad  = LatDeg * M_PI / 180.0;
        const double FracX   = (LonDeg + 180.0) / 360.0 * N;
        const double FracY   = (1.0 - std::log(std::tan(LatRad) + 1.0 / std::cos(LatRad)) / M_PI) / 2.0 * N;

        TileX  = static_cast<int>(std::floor(FracX));
        TileY  = static_cast<int>(std::floor(FracY));
        PixelX = (FracX - TileX) * 256.0;
        PixelY = (FracY - TileY) * 256.0;

        // Clamp to valid tile range
        if (TileX < 0) TileX = 0; else if (TileX >= N) TileX = N - 1;
        if (TileY < 0) TileY = 0; else if (TileY >= N) TileY = N - 1;
    }

    QRgb TexturePacker::SampleBilinear(const QImage& Img, double Px, double Py)
    {
        const int W = Img.width(), H = Img.height();
        const int X0 = static_cast<int>(Px),        Y0 = static_cast<int>(Py);
        const int X1 = qMin(X0 + 1, W - 1),         Y1 = qMin(Y0 + 1, H - 1);
        const double Tx = Px - X0,                   Ty = Py - Y0;

        const QRgb C00 = Img.pixel(X0, Y0);
        const QRgb C10 = Img.pixel(X1, Y0);
        const QRgb C01 = Img.pixel(X0, Y1);
        const QRgb C11 = Img.pixel(X1, Y1);

        auto Lerp = [](double A, double B, double T) { return A + (B - A) * T; };

        const int R = static_cast<int>(Lerp(Lerp(qRed(C00),   qRed(C10),   Tx),
                                            Lerp(qRed(C01),   qRed(C11),   Tx), Ty));
        const int G = static_cast<int>(Lerp(Lerp(qGreen(C00), qGreen(C10), Tx),
                                            Lerp(qGreen(C01), qGreen(C11), Tx), Ty));
        const int B = static_cast<int>(Lerp(Lerp(qBlue(C00),  qBlue(C10),  Tx),
                                            Lerp(qBlue(C01),  qBlue(C11),  Tx), Ty));
        return qRgb(qBound(0, R, 255), qBound(0, G, 255), qBound(0, B, 255));
    }

    const QImage* TexturePacker::FetchImageryTile(int Z, int TileX, int TileY)
    {
        const quint64 Key = (static_cast<quint64>(Z) << 42)
                          | (static_cast<quint64>(TileX & 0x1FFFFF) << 21)
                          |  static_cast<quint64>(TileY & 0x1FFFFF);

        // Fast-path: already know it's missing
        if (mMissing.contains(Key)) return nullptr;

        if (mTileCache.contains(Key))
            return &mTileCache[Key];

        const QString Base = QString("%1/%2/%3/%4").arg(mAlbedoDir).arg(Z).arg(TileX).arg(TileY);
        for (const char* Ext : { ".jpg", ".jpeg", ".png", ".webp" })
        {
            QImage Img(Base + Ext);
            if (!Img.isNull())
            {
                mTileCache.insert(Key, Img.convertToFormat(QImage::Format_RGB32));
                return &mTileCache[Key];
            }
        }

        mMissing.insert(Key, true); // remember as absent to skip future lookups
        return nullptr;
    }

    QImage TexturePacker::Pack(const GeoBounds& Bounds, int ZoomOverride)
    {
        const int Z = (ZoomOverride >= 0) ? ZoomOverride : mZoom;
        QImage Output(mOutputSize, mOutputSize, QImage::Format_RGB32);
        Output.fill(Qt::darkGray);

        const double InvSize = 1.0 / (mOutputSize - 1);

        for (int PxRow = 0; PxRow < mOutputSize; ++PxRow)
        {
            // V=0 maps to south edge, V=1 to north edge
            const double V   = 1.0 - static_cast<double>(PxRow) * InvSize;
            const double Lat = Bounds.MinLat + V * Bounds.LatSpan();

            for (int PxCol = 0; PxCol < mOutputSize; ++PxCol)
            {
                const double U   = static_cast<double>(PxCol) * InvSize;
                const double Lon = Bounds.MinLon + U * Bounds.LonSpan();

                // Clamp to valid Web-Mercator range
                const double SafeLat = qBound(-85.0511, Lat, 85.0511);

                int    TX{0}, TY{0};
                double PixX{0}, PixY{0};
                LatLonToTileXY(SafeLat, Lon, Z, TX, TY, PixX, PixY);

                const QImage* Tile = FetchImageryTile(Z, TX, TY);

                QRgb Color;
                if (Tile)
                    Color = SampleBilinear(*Tile, PixX, PixY);
                else
                    Color = qRgb(64, 64, 64);

                Output.setPixel(PxCol, PxRow, Color);
            }
        }

        return Output;
    }

} // namespace Canavar::Globe
