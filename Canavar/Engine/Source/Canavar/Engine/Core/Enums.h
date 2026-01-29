#pragma once

namespace Canavar::Engine
{
    enum class ShaderType
    {
        Model,
        Sky,
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
        Text3D,
        Cinematic,
        Aces,
        Screen,
        Bloom,
        Blur,
        FXAA,
        ColorGrading,
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

    enum class RenderPass
    {
        Opaque,
        Transparent,
    };

}