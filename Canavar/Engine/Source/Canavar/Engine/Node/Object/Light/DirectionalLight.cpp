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

void Canavar::Engine::DirectionalLight::SetDirectionFromThetaPhi(float theta, float phi)
{
    const auto r = mDirection.length();
    const auto x = r * std::cos(qDegreesToRadians(phi)) * std::cos(qDegreesToRadians(theta));
    const auto y = r * std::sin(qDegreesToRadians(phi));
    const auto z = r * std::cos(qDegreesToRadians(phi)) * std::sin(qDegreesToRadians(theta));

    mDirection = QVector3D(x, y, z);
}

void Canavar::Engine::DirectionalLight::ToJson(QJsonObject &object)
{
    Light::ToJson(object);

    QJsonObject direction;
    direction.insert("x", mDirection.x());
    direction.insert("y", mDirection.y());
    direction.insert("z", mDirection.z());
    object.insert("direction", direction);

    object.insert("radience", mRadiance);
}

void Canavar::Engine::DirectionalLight::FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes)
{
    Light::FromJson(object, nodes);

    QJsonObject defaultDirection;
    defaultDirection.insert("x", 0);
    defaultDirection.insert("y", 1);
    defaultDirection.insert("z", 0);

    QJsonObject direction = object["direction"].toObject(defaultDirection);
    float x = direction["x"].toDouble();
    float y = direction["y"].toDouble();
    float z = direction["z"].toDouble();

    mDirection = QVector3D(x, y, z);

    mRadiance = object["radiance"].toDouble(5.0);
}
