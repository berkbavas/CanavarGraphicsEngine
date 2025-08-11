#pragma once

#include <QQuaternion>
#include <QVector3D>

namespace Canavar::Simulator
{
    struct PrimaryFlightData
    {
        double AngleOfAttack;
        double SideSlip;
        double Roll;
        double Pitch;
        double Heading;
        double Airspeed;
        double MachNumber;
        double ClimbRate;
        double Latitude;
        double Longitude;
        double Altitude;
        double Pressure;
        double TurnRate;
        double SlipSkid;
        QVector3D Position;
        QQuaternion Rotation;
        double RudderPos;
        double ElevatorPos;
        double LeftAileronPos;
        double RightAileronPos;
    };
}
