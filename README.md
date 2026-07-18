# Canavar Graphics Engine

Canavar Graphics Engine is a basic graphics engine written in **C++** using the **OpenGL API** and **Qt 6**.
It leverages Qt 6 for window and event management, and utilizes Qt's math module for all 3D math operations.
The engine is designed to be modular and extensible, supporting a variety of modern rendering features and tools for 3D graphics applications.

## Features

- [x] Supports loading GLTF/OBJ/PLY 3D model formats (powered by [assimp](https://github.com/assimp/assimp))
- [x] Parent-child node hierarchy
- [x] Procedural terrain generation with tessellation
- [x] Sky with atmospheric scattering
- [x] Haze effects
- [x] Point lights
- [x] Directional lights
- [ ] Spot lights
- [x] Free camera and persecutor camera modes
- [ ] Transformation of individual meshes within models
- [x] Primitive models (Circle, Disk, Line, Plane, Sphere)
- [x] Gizmo for interactive object manipulation
- [x] Scene import/export (JSON)
- [x] Opaque and transparent rendering passes
- [x] Editor with ImGui integration
- [x] Physically Based Rendering (PBR) and Phong shading
- [x] Post-processing effects pipeline:
  - [x] ACES tone mapping
  - [x] Depth of Field
  - [x] FXAA anti-aliasing
  - [x] Color Grading
  - [x] Sharpen
  - [x] Chromatic Aberration
  - [x] Lens Distortion
  - [x] Cinematic effect
  - [ ] Bloom
  - [ ] Motion Blur
  - [ ] Screen Space Ambient Occlusion (SSAO)
  - [ ] Screen Space Reflections (SSR)
- [ ] Point Shadows
- [ ] Directional Shadows
- [ ] Spot Shadows
- [ ] Deferred rendering
- [ ] Forward+ rendering
- [ ] Ray tracing
- [ ] Global illumination
- [ ] Volumetric lighting
- [ ] Terrain LOD (Level of Detail)
- [ ] Terrain texturing with splat maps
- [ ] Particle generator
- [ ] Volumetric clouds
- [ ] Water rendering
- [ ] WGS84 ellipsoid support
- [ ] Terrain generation using DTED and satellite images

## Demo Videos

[Demo 1](https://github.com/user-attachments/assets/0a6363a7-4621-4a6f-945b-afdffa6364dd)

[Demo 2](https://github.com/user-attachments/assets/fee40d6c-3c14-4415-be26-53a90adbbdbd)

[Demo 3](https://github.com/user-attachments/assets/dd48ee1e-c6e5-4ea7-96df-57794a7cfdb1)

## Build Instructions

1. Install **CMake 3.25.1** or newer.
2. Install **Visual Studio 2022** and the **MSVC 2022 C++ Compiler**.
3. Install **Qt 6.x.y MSVC2022 64bit** kit.
4. Set the environment variable `Qt6_DIR` to the path of your Qt installation (e.g., `C:/Qt/6.x.y/msvc2022_64/lib/cmake/Qt6`).
5. Clone the repository:

   ```sh
   git clone https://github.com/berkbavas/CanavarGraphicsEngine.git
   ```

6. Create a build directory:

   ```sh
   mkdir Build
   ```

7. Navigate into the build directory:

   ```sh
   cd Build
   ```

8. Run CMake to configure the project:

   ```sh
   cmake ..
   ```

9. Open the generated solution file (`Canavar.sln`) with **Visual Studio 2022**.
10. Edit `MODELS_FOLDER` variable in `Source/Canavar/Engine/Core/Constants.h` to point to your models directory. Supported formats: GLTF, GLB, OBJ, PLY.
11. Set **Editor** as the startup project.
12. Build and run the project using the **Release** configuration.

## Canavar Engine Module

The **Engine** module is the core of the Canavar Graphics Engine, providing all essential systems for 3D rendering, scene management, and extensibility.
It is written in C++ and leverages **Qt 6** for windowing, math, and event handling, and **OpenGL 4.5** for graphics.

### Directory Structure

- `Source/Canavar/Engine/Camera/` — Camera types (FreeCamera, PersecutorCamera, PerspectiveCamera)
- `Source/Canavar/Engine/Core/` — Core engine classes (OpenGLWidget, Shader, Framebuffer, RenderingContext, etc.)
- `Source/Canavar/Engine/GlobalNode/` — Scene-wide singleton nodes (Sky, Haze, Terrain)
- `Source/Canavar/Engine/Light/` — Light types (DirectionalLight, PointLight)
- `Source/Canavar/Engine/Manager/` — Managers for rendering, nodes, lights, cameras, etc. (Renderer, NodeManager, LightManager, CameraManager, TexturedModelRenderer, PrimitiveModelRenderer)
- `Source/Canavar/Engine/Model/` — Model types and materials (TexturedModel, PrimitiveModel, PbrMaterial, PhongMaterial)
- `Source/Canavar/Engine/Node/` — Base `Node` class with hierarchy support
- `Source/Canavar/Engine/Object/` — `Object` base class (scene objects with transform)
- `Source/Canavar/Engine/PostProcessEffect/` — Post-processing effects (ACES, DoF, FXAA, Color Grading, Sharpen, etc.)
- `Source/Canavar/Engine/Scene/` — Scene graph internals (Scene, SceneNode, Mesh, TextureMaterial)
- `Source/Canavar/Engine/Util/` — Utilities (Logger, AssimpModelImporter, ImGuiWidget, Gizmo, etc.)
- `Resources/Shaders/` — GLSL shaders used by the engine

### Dependencies

- [Qt 6](https://www.qt.io/)
- [OpenGL 4.5](https://www.khronos.org/opengl/)
- [assimp](https://github.com/assimp/assimp)
- [freetype](https://freetype.org/)
- [qtimgui](https://github.com/seanchas116/qtimgui)

### Usage

To use the Engine module in your application:

1. Create an `OpenGLWidget` as the rendering surface.
2. Instantiate a `Renderer` with the widget.
3. Optionally create an `ImGuiWidget` for the built-in editor UI.
4. Use the managers (`NodeManager`, `CameraManager`, etc.) to set up your scene.
5. Implement your own logic by connecting to `Renderer` signals (`Initialized`, `Updated`, `PostRender`).

Example:

```cpp
#include <Canavar/Engine/Core/OpenGLWidget.h>
#include <Canavar/Engine/Manager/Renderer.h>
#include <Canavar/Engine/Util/ImGuiWidget.h>

void MyClass::Run()
{
    // Create the OpenGL rendering surface
    mOpenGLWidget = new Canavar::Engine::OpenGLWidget(nullptr);

    // Create the renderer (owns NodeManager, CameraManager, LightManager, etc.)
    mRenderer = std::make_unique<Canavar::Engine::Renderer>(mOpenGLWidget);

    // Optionally attach the built-in ImGui editor
    mImGuiWidget = std::make_unique<Canavar::Engine::ImGuiWidget>(mRenderer.get());

    // Setup connections
    connect(mRenderer.get(), &Canavar::Engine::Renderer::Initialized, this, &MyClass::Initialize);
    connect(mRenderer.get(), &Canavar::Engine::Renderer::Updated,     this, &MyClass::Update);
    connect(mRenderer.get(), &Canavar::Engine::Renderer::PostRender,  this, &MyClass::PostRender);

    mOpenGLWidget->showMaximized();
}

void MyClass::Initialize()
{
    mRenderer->GetNodeManager()->ImportNodes("/path/to/your/scene.json");
}

void MyClass::Update(float ifps)
{
    // Update code here
}

void MyClass::PostRender(float ifps)
{
    // Post-render code here (e.g. custom ImGui windows)
}
```

## Acknowledgements

- Joey de Vries for OpenGL tutorials
- Contributors to Qt, assimp, freetype, JSBSim

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
