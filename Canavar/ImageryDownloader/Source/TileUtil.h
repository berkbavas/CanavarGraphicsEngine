#pragma once

#include <QString>
#include <cmath>
#include <utility>

// Web Mercator (EPSG:3857) / XYZ tile utilities.
namespace TileUtil
{
    // Convert geographic coordinates to tile indices at a given zoom level.
    // Latitude must be in [-85.051129, 85.051129] (Web Mercator limits).
    inline std::pair<int, int> latLonToTile(double lat, double lon, int z)
    {
        const int n = 1 << z;
        const double latRad = lat * M_PI / 180.0;

        int x = static_cast<int>(std::floor((lon + 180.0) / 360.0 * n));
        int y = static_cast<int>(std::floor(
            (1.0 - std::log(std::tan(latRad) + 1.0 / std::cos(latRad)) / M_PI) / 2.0 * n));

        x = std::clamp(x, 0, n - 1);
        y = std::clamp(y, 0, n - 1);
        return { x, y };
    }

    // Number of tiles per axis at zoom level z.
    inline int tileCount(int z) { return 1 << z; }

    // Substitute {z}, {x}, {y} placeholders in a URL template.
    inline QString buildUrl(const QString& urlTemplate, int z, int x, int y)
    {
        return QString(urlTemplate)
            .replace("{z}", QString::number(z))
            .replace("{x}", QString::number(x))
            .replace("{y}", QString::number(y));
    }

    // Bounding tile range for a lat/lon region.
    struct TileRange
    {
        int xMin, xMax, yMin, yMax;
        int tileCount() const { return (xMax - xMin + 1) * (yMax - yMin + 1); }
    };

    // maxLat/minLon give the top-left corner; minLat/maxLon give the bottom-right.
    inline TileRange computeRange(double minLat, double maxLat,
                                  double minLon, double maxLon, int z)
    {
        auto [x0, y0] = latLonToTile(maxLat, minLon, z); // top-left  (y smallest)
        auto [x1, y1] = latLonToTile(minLat, maxLon, z); // btm-right (y largest)
        return { x0, x1, y0, y1 };
    }
} // namespace TileUtil
