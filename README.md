# Canavar Graphics Engine

Canavar Graphics Engine is a basic graphics engine written in **C++** using the **OpenGL API** and **Qt 6**. It leverages Qt 6 for window and event management, and utilizes Qt's math module for all 3D math operations. The engine is designed to be modular and extensible, supporting a variety of modern rendering features and tools for 3D graphics applications.

## Features

- ✅ Supports loading several 3D model formats (powered by [assimp](External/assimp/))
- ✅ Parent-child node hierarchy
- ✅ Procedural terrain generation
- ✅ Sky with atmospheric scattering
- ✅ Haze effects
- ✅ Point lights
- ✅ Directional lights
- ⬜ Spot lights (planned)
- ✅ Free camera and persecutor camera modes
- ✅ Transformation of individual meshes within models
- ✅ Lightning Strike Generator
- ✅ Nozzle Effect
- ✅ Shadow Mapping
- ⬜ Point Shadows (planned)
- ⬜ Particle generator (planned)
- ⬜ Volumetric clouds (planned)
- ⬜ Water rendering (planned)
- ✅ Vertex Painting
- ✅ Editor with ImGui integration
- ✅ Physically Based Rendering (PBR)
- ⬜ WGS84 ellipsoid support (planned)
- ⬜ Terrain generation using DTED and satellite images (planned)
- ⬜ Post processing effects (planned)

## Videos

- [Demo 1](https://github.com/user-attachments/assets/72ca55f4-0f0b-48f5-be76-a3ab7fb35183)
- [Demo 2](https://github.com/user-attachments/assets/e6a14a70-482f-449f-ad58-2ef4f8164648)
- [Demo 3](https://github.com/user-attachments/assets/52296861-428c-4694-bbe4-b03f44dd3d18)
- [Demo 4](https://github.com/user-attachments/assets/a3a1cd6e-0239-4a83-b0b9-13c3846d6698)
- [Demo 5](https://github.com/user-attachments/assets/04041241-4dd9-49b1-92ea-0e0b61b3369f)
- [Demo 6](https://github.com/user-attachments/assets/d73b6242-a21e-405a-9e01-acbe09288390)
- [Demo 7](https://github.com/user-attachments/assets/186846ec-5881-4168-9b4c-83ce20c04f80)

## Build Instructions

1. Install **CMake 3.25.1**.
2. Install **Visual Studio 2022** and the **MSVC C++ Compiler**.
3. Install **Qt 6.7.3 MSVC2022 64bit** kit.
4. Clone the repository:

   ```sh
   git clone https://github.com/berkbavas/CanavarGraphicsEngine.git
   ```

5. Create a build directory:

   ```sh
   mkdir Build
   ```

6. Navigate into the build directory:

   ```sh
   cd Build
   ```

7. Run CMake to configure the project:

   ```sh
   cmake ..
   ```

8. Open the generated solution file (`Canavar.sln`) with **Visual Studio 2022**.
9. Build and run the project using the **Release** configuration.

## Canavar Engine Module

The **Engine** module is the core of the Canavar Graphics Engine, providing all essential systems for 3D rendering, scene management, and extensibility. It is written in modern C++ and leverages **Qt 6** for windowing, math, and event handling, and **OpenGL 4.5** for graphics.

### Features

- Scene graph and node hierarchy
- Support for multiple 3D model formats (via [assimp](../../External/assimp/))
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
- [assimp](../../External/assimp/)
- [freetype](../../External/freetype/)
- [JSBSim](../../External/JSBSim/)
- [qtimgui](../../External/qtimgui/)


### Usage

To use the Engine module in your application:

1. Create a `RenderingContext` (e.g., `Window` or `Widget`).
2. Instantiate a `Controller` with the rendering context.
3. Use the managers (NodeManager, RenderingManager, etc.) to set up your scene.
4. Implement your own nodes or extend existing ones for custom behavior.

## Acknowledgements

- Joey de Vries for OpenGL tutorials
- Contributors to Qt, assimp, freetype, JSBSim
