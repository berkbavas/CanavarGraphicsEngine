# Canavar Graphics Engine

A basic graphics engine written with `C++` using `OpenGL API`.
I use `Qt 6` for window and event management.
All 3D math operation is done by `Qt 6`'s math module.
The engine has following features:
- Supports loading several 3D model formats thanks to `assimp` library
- Parent - child node hiearchy
- Transformation of individual meshes of models
- Particle generator
- Terrain, sky, sun, volumetric clouds, water, haze
- Dummy and free cameras
- Directional, point and spot lights
- Easy shader loading and linking
- Framebuffer generation with several attachments and different formats 

I will add following features:
- Editor
- WGS84 support
- Terrain generation using DTED
- Albedo generation using satellite images
- Post processing effects

## Screenshot
![Screenshot_2022_10_23](https://user-images.githubusercontent.com/53399385/210186793-719e9705-a4c3-439a-89b6-d2181f6b91d7.png)

## Build
1) Install `CMake 3.25.1`.
2) Install `Visual Studio 2019 and MSVC C++ Compiler`.
3) Install `Qt 6.4.1 MSVC2019 64bit` kit.
4) Clone the repo `git clone https://github.com/berkbavas/CanavarGraphicsEngine.git`.
5) Create a folder `mkdir Build`.
6) Enter the folder `cd Build`.
7) Run CMake `cmake ..`.
8) Open `Canavar.sln` with `Visual Studio 2019`.
9) Build & Run with `Release` config.

## Usage
Creating a model can be done with one liner:
```
Node *aircraft = NodeManager::Instance()->CreateModel("f16c");
```
`NodeManager` class automatically register this model to render list. You do not have to do anything else.
Ownership belongs to `NodeManager`. You can remove any node with one liner again.
```
NodeManager::Instance()->RemoveNode(aircraft);
```
It is `NodeManager`'s responsibility to the clean up the resources.

Attaching a camera to a node is also easy:
```
Camera *camera = NodeManager::Instance()->Create(Node::NodeType::DummyCamera);
aircraft->AddChild(camera);
```
You can set local position of `camera`:

```
camera->SetPosition(QVector3D(0, 0, 10));
```
Now you can set `camera` as the active camera:

```
CameraManager::Instance()->SetActiveCamera(camera);
```

## Acknowledgements
I thank Federico Vaccaro for his amazing shaders and Joey de Vries for his OpenGL tutorials.
