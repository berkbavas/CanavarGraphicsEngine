#pragma once

namespace Canavar::Engine
{
    enum class ShaderType
    {
        Model,
        Sky,
        Blur,
        PostProcess,
        NozzleEffect,
        Line,
        LightningStrike,
        ShadowMapping,
        Bitmap,
        PointCloud,
        Basic,
        CrossSection,
        Terrain,
        Text2D,
        Text3D
    };

    enum class ViewDirection
    {
        Front,
        Back,
        Up,
        Down,
        Left,
        Right
    };

}