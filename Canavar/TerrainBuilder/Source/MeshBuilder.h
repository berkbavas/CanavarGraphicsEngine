#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  MeshBuilder
//  Generates a terrain mesh on the WGS-84 ellipsoid surface for a given
//  geographic tile.  Vertex positions are ECEF metres relative to the tile
//  centre (for float-precision rendering).
// ─────────────────────────────────────────────────────────────────────────────

#include "TerrainCommon.h"

#include <QVector>
#include <array>

namespace Canavar::Globe
{
    class ElevationSampler;

    class MeshBuilder
    {
      public:
        // GridRes: vertices per side (e.g., 65 → 64×64 quads). Recommended: 33, 65.
        explicit MeshBuilder(int GridRes = 65);

        // Build mesh for the given tile. Returns false if sampler has no data.
        // Outputs:
        //   VerticesOut  – one entry per grid vertex
        //   IndicesOut   – triangle-list indices (CCW winding)
        //   CenterEcef   – ECEF position of tile centre
        bool Build(const GeoBounds&    Bounds,
                   ElevationSampler&   Sampler,
                   QVector<TerrainVertex>&        VerticesOut,
                   QVector<uint32_t>&             IndicesOut,
                   std::array<double, 3>&         CenterEcefOut);

      private:
        int mGridRes;
    };

} // namespace Canavar::Globe
