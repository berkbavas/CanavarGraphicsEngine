#pragma once

namespace Canavar::Simulator
{
    enum class Command
    {
        Aileron,
        Elevator,
        Rudder,
        Steering,
        Flaps,
        Speedbrake,
        Spoiler,
        PitchTrim,
        RudderTrim,
        AileronTrim,
        Throttle,
        Mixture,
        Gear,
        PropellerPitch,
        PropellerFeather,
        InitRunning,
        Hold,
        Resume,
    };
}
