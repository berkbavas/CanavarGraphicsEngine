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
        Ping,
        Pong,
    };

    enum class PostProcessEffectType
    {
        None,
        DepthOfField,        // Bokeh depth of field (runs first, before tone mapping)
        Aces,                // ACES tone mapping (HDR -> LDR)
        ColorGrading,        // Brightness / contrast / saturation / temperature
        Sharpen,             // Unsharp-mask image sharpening
        Fxaa,                // Fast Approximate Anti-Aliasing
        ChromaticAberration, // RGB channel separation at screen edges
        LensDistortion,      // Barrel / pincushion lens distortion
        Cinematic,           // Vignette + film grain (applied last)
    };

    enum class PrimitiveType
    {
        Circle, // Line-loop circle in the XZ plane; has screen-space thickness
        Disk,   // Filled disk in the XZ plane
        Line,   // Line segment from origin to +Z; has screen-space thickness
        Plane,  // Unit quad in the XZ plane
        Sphere, // UV sphere of radius 1
    };
}