# Canavar Graphics Engine
A basic graphics engine written in `C++` using `OpenGL API` and `Qt 6`.
I use `Qt 6` for window and event management.
All 3D math operations are handled by `Qt 6`'s math module.

## Features
- [x] Supports loading several 3D model formats, thanks to `assimp` library
- [x] Parent - child node hierarchy
- [x] Procedural terrain
- [x] Sky with atmospheric scattering
- [x] Haze
- [x] Point lights
- [x] Directional lights
- [ ] Spot lights
- [x] Free camera
- [x] Persecutor camera
- [x] Transformation of individual meshes of models
- [x] Lightning Strike Generator
- [ ] Particle generator
- [ ] Volumetric clouds
- [ ] Water
- [ ] Editor
- [ ] WGS84 ellipsoid support
- [ ] Terrain generation using DTED and satellite images
- [ ] Post processing effects

## Video

https://github.com/user-attachments/assets/52296861-428c-4694-bbe4-b03f44dd3d18

## Build
1) Install `CMake 3.25.1`.
2) Install `Visual Studio 2022 and MSVC C++ Compiler`.
3) Install `Qt 6.7.3 MSVC2022 64bit` kit.
4) Clone the repo `git clone https://github.com/berkbavas/CanavarGraphicsEngine.git`.
5) Create a folder `mkdir Build`.
6) Enter the folder `cd Build`.
7) Run CMake `cmake ..`.
8) Open `Canavar.sln` with `Visual Studio 2019`.
9) Build & Run with `Release` config.

## Acknowledgements
I thank Federico Vaccaro for his amazing shaders and Joey de Vries for his OpenGL tutorials.
