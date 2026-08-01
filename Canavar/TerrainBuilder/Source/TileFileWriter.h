#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  TileFileWriter  –  writes a TerrainTileHeader + mesh + JPEG texture to disk.
// ─────────────────────────────────────────────────────────────────────────────

#include "TerrainCommon.h"

#include <QVector>
#include <QImage>
#include <QString>
#include <array>

namespace Canavar::Globe
{
    class TileFileWriter
    {
      public:
        // JpegQuality: 0–100 for the embedded texture.
        explicit TileFileWriter(const QString& OutputRoot, int JpegQuality = 85);

        bool Write(const TileId&                     Id,
                   const GeoBounds&                  Bounds,
                   const std::array<double, 3>&      CenterEcef,
                   const QVector<TerrainVertex>&     Vertices,
                   const QVector<uint32_t>&          Indices,
                   const QImage&                     Texture);

        // Path to the .tbin file for the given tile (does NOT imply file exists).
        QString TilePath(const TileId& Id) const;

        bool TileExists(const TileId& Id) const;

      private:
        QString mOutputRoot;
        int     mJpegQuality;
    };

} // namespace Canavar::Globe
