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
        BrightPass,
        GodRays,
        Composition,
        Aces,
        MotionBlur
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