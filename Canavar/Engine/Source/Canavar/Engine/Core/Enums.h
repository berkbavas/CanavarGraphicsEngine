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
        Ping, // Used for post-processing effects that require multiple passes (e.g., DOF, motion blur)
        Pong, // Used for post-processing effects that require multiple passes (e.g., DOF, motion blur)
    };

    // Order of post-processing effects matters, as some effects depend on the output of previous effects.
    enum class PostProcessEffectType
    {
        None = 0,
        AerialPerspective = 1,   // Rayleigh+Mie scattering on scene objects (runs first, in HDR)
        DepthOfField = 2,        // Bokeh depth of field (runs before tone mapping)
        Aces = 3,                // ACES tone mapping (HDR -> LDR)
        ColorGrading = 4,        // Brightness / contrast / saturation / temperature
        Sharpen = 5,             // Unsharp-mask image sharpening
        Fxaa = 6,                // Fast Approximate Anti-Aliasing
        ChromaticAberration = 7, // RGB channel separation at screen edges
        LensDistortion = 8,      // Barrel / pincushion lens distortion
        Cinematic = 9,           // Vignette + film grain (applied last)
    };

    enum class PrimitiveType
    {
        Circle, // Line-loop circle in the XZ plane; has screen-space thickness
        Disk,   // Filled disk in the XZ plane
        Line,   // Line segment from origin to +Z; has screen-space thickness
        Plane,  // Unit quad in the XZ plane
        Sphere, // UV sphere of radius 1
    };

    enum class ImGuiTheme
    {
        Dark = 0,
        Light = 1,
        Dracula = 2,
        Midnight = 3,
    };

}