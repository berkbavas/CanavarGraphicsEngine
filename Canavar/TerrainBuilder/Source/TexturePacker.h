#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  TexturePacker
//  Assembles a terrain texture for a geographic tile by compositing XYZ imagery
//  tiles stored on disk.  Performs proper Web-Mercator → Geographic reprojection
//  so that the texture maps accurately onto the ellipsoid mesh.
// ─────────────────────────────────────────────────────────────────────────────

#include "TerrainCommon.h"

#include <QImage>
#include <QHash>
#include <QString>

namespace Canavar::Globe
{
    class TexturePacker
    {
      public:
        // AlbedoDir: root directory containing {z}/{x}/{y}.jpg (or .png)
        // ZoomLevel: imagery zoom level to use (typically 10–14)
        // OutputSize: width and height of the produced texture (power-of-2 recommended)
        explicit TexturePacker(const QString& AlbedoDir,
                               int            ZoomLevel  = 12,
                               int            OutputSize = 512);

        // Build a texture for the given geographic bounds.
        // ZoomOverride >= 0 overrides the construction-time zoom level.
        QImage Pack(const GeoBounds& Bounds, int ZoomOverride = -1);

        void SetZoom(int Zoom) { mZoom = Zoom; }

      private:
        // Load (or return cached) an imagery tile.
        const QImage* FetchImageryTile(int Z, int TileX, int TileY);

        // Web-Mercator helpers
        static void  LatLonToTileXY(double LatDeg, double LonDeg, int Z,
                                     int& TileX, int& TileY,
                                     double& PixelX, double& PixelY);
        static QRgb  SampleBilinear(const QImage& Img, double Px, double Py);

        QString mAlbedoDir;
        int     mZoom;
        int     mOutputSize;

        QHash<quint64, QImage> mTileCache;  // key → loaded tile (null QImage = not found)
        QHash<quint64, bool>   mMissing;    // fast set for confirmed-absent tiles
    };

} // namespace Canavar::Globe
