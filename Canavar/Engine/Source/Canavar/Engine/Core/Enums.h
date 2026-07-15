#pragma once

namespace Canavar::Engine
{
    enum class RenderPass
    {
        Opaque = 0,
        Transparent = 1,

    };

    enum class ShadingMode
    {
        Pbr = 0,
        Phong = 1,
    };

    enum class TextureType
    {
        BaseColor,
        Normal,
        Metallic,
        Roughness,
        AmbientOcclusion
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

    enum FramebufferType
    {
        Multisample,
        Singlesample,
    };

}