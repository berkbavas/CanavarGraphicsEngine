#include "DirectionalLight.h"

#include <cmath>

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

void Canavar::Engine::DirectionalLight::SetDirectionFromThetaPhi(float Theta, float Phi)
{
    const auto r = mDirection.length();
    const auto x = r * std::cos(qDegreesToRadians(Phi)) * std::cos(qDegreesToRadians(Theta));
    const auto y = r * std::sin(qDegreesToRadians(Phi));
    const auto z = r * std::cos(qDegreesToRadians(Phi)) * std::sin(qDegreesToRadians(Theta));

    mDirection = QVector3D(x, y, z);
}

void Canavar::Engine::DirectionalLight::ToJson(QJsonObject &Object)
{
    Light::ToJson(Object);

    QJsonObject Direction;
    Direction.insert("x", mDirection.x());
    Direction.insert("y", mDirection.y());
    Direction.insert("z", mDirection.z());
    Object.insert("direction", Direction);

    Object.insert("radiance", mRadiance);
}

void Canavar::Engine::DirectionalLight::FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes)
{
    Light::FromJson(Object, Nodes);

    QJsonObject DefaultDirection;
    DefaultDirection.insert("x", 0);
    DefaultDirection.insert("y", 1);
    DefaultDirection.insert("z", 0);

    QJsonObject Direction = Object["direction"].toObject(DefaultDirection);
    float x = Direction["x"].toDouble();
    float y = Direction["y"].toDouble();
    float z = Direction["z"].toDouble();

    mDirection = QVector3D(x, y, z);

    mRadiance = Object["radiance"].toDouble(5.0);
}
