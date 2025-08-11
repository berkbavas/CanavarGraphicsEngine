#pragma once

namespace Canavar::Simulator
{

    inline constexpr const char* RESOURCE_PATH{ "Resources/Data/" };
    inline constexpr const char* JSBSIM_MODEL_PATH{ "c172p" };
    inline constexpr const char* JSBSIM_INITIAL_CONDITIONS_PATH{ "reset00.xml" };

    inline constexpr float FEET_TO_METER{ 0.3048f };
    inline constexpr float METER_TO_FEET{ 1 / FEET_TO_METER };

    inline constexpr float DELTA_TIME{ 1 / 60.0f };
    inline constexpr float INV_DELTA_TIME{ 1.0f / DELTA_TIME };

    inline constexpr float TERRAIN_ELEVATION{ 0.0f };
}