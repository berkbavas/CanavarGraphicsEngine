#include "DirectionalLight.h"

Canavar::Engine::DirectionalLight::DirectionalLight()
{
    SetNodeName("Directional Light");
}

float Canavar::Engine::DirectionalLight::GetTheta() const
{
    const auto z = mDirection.z();
    const auto x = mDirection.x();
    return qRadiansToDegrees(atan2(z, x));
}

float Canavar::Engine::DirectionalLight::GetPhi() const
{
    const auto x = mDirection.x();
    const auto y = mDirection.y();
    const auto z = mDirection.z();

    return qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));
}

void Canavar::Engine::DirectionalLight::SetDirectionFromThetaPhi(float theta, float phi)
{
    const auto r = mDirection.length();
    const auto x = r * cos(qDegreesToRadians(phi)) * cos(qDegreesToRadians(theta));
    const auto y = r * sin(qDegreesToRadians(phi));
    const auto z = r * cos(qDegreesToRadians(phi)) * sin(qDegreesToRadians(theta));

    mDirection = QVector3D(x, y, z);
}