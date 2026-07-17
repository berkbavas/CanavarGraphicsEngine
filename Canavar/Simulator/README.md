# Canavar Simulator Module

The **Simulator** module extends the Canavar Graphics Engine with advanced flight simulation capabilities. It integrates with the [JSBSim](https://github.com/JSBSim-Team/jsbsim) flight dynamics model to provide realistic aircraft physics, control systems, and simulation features.

## Features

- Realistic flight dynamics powered by JSBSim
- Aircraft initialization and control
- Support for multiple aircraft models
- Primary Flight Display (PFD) data extraction
- Autopilot and manual control modes
- Integration with the Canavar Engine’s rendering and input systems
- ImGui-based GUI for simulation control and monitoring
- Extensible command and event system

## Dependencies

- [JSBSim](../../External/JSBSim/)
- [Qt 6](https://www.qt.io/)
- [OpenGL 4.5](https://www.khronos.org/opengl/)
- [ImGui](../../External/qtimgui/)
- [assimp](../../External/assimp/)

## Building

The Simulator module is built as part of the main Canavar Graphics Engine project. Please refer to the [top-level README](../../README.md) for build instructions and prerequisites.

## Usage

1. Initialize the Simulator module alongside the Engine module.
2. Create and configure an `Aircraft` instance.
3. Use the provided GUI to control the simulation, monitor flight data, and interact with the aircraft.
4. Extend the module by adding new aircraft models or custom simulation logic as needed.

## Example

```cpp
#include <Canavar/Simulator/Aircraft.h>

// Create and initialize an aircraft
Canavar::Simulator::Aircraft* aircraft = new Canavar::Simulator::Aircraft();
aircraft->Initialize();

// In your main loop
aircraft->Tick(deltaTime);
aircraft->DrawGui();
```

## Acknowledgements

- [JSBSim](https://github.com/JSBSim-Team/jsbsim) for flight dynamics modeling
- Contributors to Qt, ImGui, and other open-source libraries used in this