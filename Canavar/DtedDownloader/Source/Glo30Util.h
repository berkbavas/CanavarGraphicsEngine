#pragma once

#include <QString>
#include <cmath>

// URL and filename helpers for the Copernicus DEM GLO-30 AWS Open Data dataset.
// Tiles are 1°×1°, publicly accessible at:
//   https://copernicus-dem-30m.s3.amazonaws.com/<stem>/<stem>.tif
namespace Glo30Util
{
    // Returns the canonical tile stem, e.g. "Copernicus_DSM_COG_10_N41_00_E028_00_DEM"
    inline QString buildStem(int lat, int lon)
    {
        char ns = lat >= 0 ? 'N' : 'S';
        char ew = lon >= 0 ? 'E' : 'W';
        return QString("Copernicus_DSM_COG_10_%1%2_00_%3%4_00_DEM")
            .arg(ns)
            .arg(std::abs(lat), 2, 10, QChar('0'))
            .arg(ew)
            .arg(std::abs(lon), 3, 10, QChar('0'));
    }

    inline QString buildUrl(int lat, int lon)
    {
        QString stem = buildStem(lat, lon);
        return QString("https://copernicus-dem-30m.s3.amazonaws.com/%1/%1.tif").arg(stem);
    }

    inline QString buildFilename(int lat, int lon)
    {
        return buildStem(lat, lon) + ".tif";
    }
} // namespace Glo30Util
