# Canavar Graphics Engine

Canavar Graphics Engine is a basic graphics engine written in **C++** using the **OpenGL API** and **Qt 6**.
It leverages Qt 6 for window and event management, and utilizes Qt's math module for all 3D math operations.
The engine is designed to be modular and extensible, supporting a variety of modern rendering features and tools for 3D graphics applications.

## Features

- [x] Supports loading GLTF 3D model format (powered by [assimp](https://github.com/assimp/assimp))
- [x] Parent-child node hierarchy
- [x] Procedural terrain generation
- [x] Sky with atmospheric scattering
- [x] Haze effects
- [x] Point lights
- [x] Directional lights
- [ ] Spot lights
- [x] Free camera and persecutor camera modes
- [x] Transformation of individual meshes within models
- [x] Lightning Strike Generator
- [x] Nozzle Effect
- [x] Shadow Mapping
- [ ] Point Shadows
- [ ] Particle generator
- [ ] Volumetric clouds
- [ ] Water rendering
- [x] Vertex Painting
- [x] Editor with ImGui integration
- [x] Physically Based Rendering (PBR)
- [ ] WGS84 ellipsoid support
- [ ] Terrain generation using DTED and satellite images
- [ ] Post processing effects

## Videos

[Demo 1](https://github.com/user-attachments/assets/72ca55f4-0f0b-48f5-be76-a3ab7fb35183)

[Demo 2](https://github.com/user-attachments/assets/e6a14a70-482f-449f-ad58-2ef4f8164648)

[Demo 3](https://github.com/user-attachments/assets/52296861-428c-4694-bbe4-b03f44dd3d18)

[Demo 4](https://github.com/user-attachments/assets/a3a1cd6e-0239-4a83-b0b9-13c3846d6698)

[Demo 5](https://github.com/user-attachments/assets/04041241-4dd9-49b1-92ea-0e0b61b3369f)

[Demo 6](https://github.com/user-attachments/assets/d73b6242-a21e-405a-9e01-acbe09288390)

[Demo 7](https://github.com/user-attachments/assets/186846ec-5881-4168-9b4c-83ce20c04f80)

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
10. Edit `MODELS_FOLDER` variable in `Source/Canavar/Engine/Core/Constants.h` to point to your models directory. Currently, I support only the GLTF format.
11. Set **Editor** as the startup project.
12. Build and run the project using the **Release** configuration.

## Canavar Engine Module

The **Engine** module is the core of the Canavar Graphics Engine, providing all essential systems for 3D rendering, scene management, and extensibility.
It is written in C++ and leverages **Qt 6** for windowing, math, and event handling, and **OpenGL 4.5** for graphics.

### Features of Engine Module

- Scene graph and node hierarchy
- Support for multiple 3D model formats (via [assimp](https://github.com/assimp/assimp))
- Physically Based Rendering (PBR) and Phong shading
- Procedural terrain generation
- Atmospheric sky and haze effects
- Point and directional lights
- Shadow mapping
- Vertex painting
- ImGui-based editor integration
- Extensible node and effect system

### Directory Structure

- `Source/Canavar/Engine/Core/` — Core engine classes (window, rendering context, controller)
- `Source/Canavar/Engine/Manager/` — Managers for rendering, nodes, shaders, lights, etc.
- `Source/Canavar/Engine/Node/` — Scene nodes (objects, lights, cameras, effects, terrain, etc.)
- `Source/Canavar/Engine/Util/` — Utilities (logging, math, ImGui integration, etc.)
- `Resources/Shaders/` — GLSL shaders used by the engine

### Dependencies

- [Qt 6](https://www.qt.io/)
- [OpenGL 4.5](https://www.khronos.org/opengl/)
- [assimp](https://github.com/assimp/assimp)
- [freetype](https://freetype.org/)
- [JSBSim](https://github.com/JSBSim-Team/jsbsim)
- [qtimgui](https://github.com/seanchas116/qtimgui)

### Usage

To use the Engine module in your application:

1. Create a `RenderingContext` (e.g., `Window` or `Widget`).
2. Instantiate a `Controller` with the rendering context.
3. Use the managers (NodeManager, RenderingManager, etc.) to set up your scene.
4. Implement your own nodes or extend existing ones for custom behavior.

Example:

```cpp
#include <Canavar/Engine/Core/Window.h>
#include <Canavar/Engine/Core/Controller.h>
#include <Canavar/Engine/Manager/NodeManager.h>

void MyClass::Run()
{
   // Create a rendering context
   mWindow = new Canavar::Engine::Window(this);

   // Initialize the controller with rendering context
   mController = new Canavar::Engine::Controller(mWindow, true, this); 

   // Setup connections
   connect(mController, &Canavar::Engine::Controller::Initialized, this, &MyClass::Initialize);
   connect(mController, &Canavar::Engine::Controller::Updated, this, &MyClass::Update);
   connect(mController, &Canavar::Engine::Controller::PostRendered, this, &MyClass::PostRender);

   mWindow->showMaximized();
}

void MyClass::Initialize()
{
    mWindow->GetNodeManager()->ImportNodes("/path/to/your/scene.json");
}

void MyClass::Update(float ifps)
{
    // Update code here
}

void MyClass::PostRender(float ifps)
{
    // Post-render code here
}
```

## Acknowledgements

- Joey de Vries for OpenGL tutorials
- Contributors to Qt, assimp, freetype, JSBSim

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
