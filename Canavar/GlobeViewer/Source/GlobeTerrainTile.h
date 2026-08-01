#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  GlobeTerrainTile  –  runtime representation of one loaded terrain tile.
//  Holds OpenGL VAO/VBO/IBO + GPU texture handle.
// ─────────────────────────────────────────────────────────────────────────────

#include "TerrainCommon.h"

#include <QOpenGLFunctions_4_5_Core>
#include <array>
#include <memory>

namespace Canavar::Globe
{
    struct GlobeTerrainTile
    {
        TileId                Id;
        GeoBounds             Bounds;
        std::array<double, 3> CenterEcef{};
        float                 GeometricError{ 0 };
        uint32_t              IndexCount{ 0 };

        GLuint Vao{ 0 };
        GLuint Vbo{ 0 };
        GLuint Ibo{ 0 };
        GLuint Texture{ 0 };

        bool IsReady() const noexcept { return Vao != 0 && Texture != 0; }
    };

    using GlobeTerrainTilePtr = std::unique_ptr<GlobeTerrainTile>;

} // namespace Canavar::Globe
