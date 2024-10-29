#include "DirectionalLight.h"

#include <cmath>

Canavar::Engine::DirectionalLight::DirectionalLight()
{
    SetNodeName("Directional Light");
}

float Canavar::Engine::DirectionalLight::GetTheta() const
{
    const auto z = mDirection.z();
    const auto x = mDirection.x();
    return qRadiansToDegrees(std::atan2(z, x));
}

float Canavar::Engine::DirectionalLight::GetPhi() const
{
    const auto x = mDirection.x();
    const auto y = mDirection.y();
    const auto z = mDirection.z();

    return qRadiansToDegrees(std::atan2(y, std::sqrt(z * z + x * x)));
}

void Canavar::Engine::DirectionalLight::SetDirectionFromThetaPhi(float theta, float phi)
{
    const auto r = mDirection.length();
    const auto x = r * std::cos(qDegreesToRadians(phi)) * std::cos(qDegreesToRadians(theta));
    const auto y = r * std::sin(qDegreesToRadians(phi));
    const auto z = r * std::cos(qDegreesToRadians(phi)) * std::sin(qDegreesToRadians(theta));

    mDirection = QVector3D(x, y, z);
}