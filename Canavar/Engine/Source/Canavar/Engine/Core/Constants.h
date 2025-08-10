#pragma once

#include <QVector3D>

namespace Canavar::Engine
{
    inline constexpr int SAMPLES = 4;
    inline constexpr int INITIAL_WIDTH = 1600;
    inline constexpr int INITIAL_HEIGHT = 900;
    inline constexpr int SHADOW_MAP_SIZE = 4096;
    inline constexpr int SHADOW_MAP_TEXTURE_UNIT = 10;
    inline constexpr const char* MODELS_FOLDER = "C:/Canavar/Models";

    inline constexpr QVector3D ORIGIN{ 0, 0, 0 };
    inline constexpr QVector3D ZERO_3D{ 0, 0, 0 };

    inline constexpr int PBR_SHADING = 0;
    inline constexpr int PHONG_SHADING = 1;

    enum class RenderPass
    {
        Opaque,
        Transparent
    };
}
