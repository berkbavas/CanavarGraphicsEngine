#include "PointLight.h"

Canavar::Engine::PointLight::PointLight()
    : Light()
    , mConstant(1.0f)
    , mLinear(0.09f)
    , mQuadratic(0.001f)
{
    mType = Canavar::Engine::Node::NodeType::PointLight;
}

void Canavar::Engine::PointLight::ToJson(QJsonObject& object)
{
    Light::ToJson(object);

    object.insert("constant", mConstant);
    object.insert("linear", mLinear);
    object.insert("quadratic", mQuadratic);
}

void Canavar::Engine::PointLight::FromJson(const QJsonObject& object)
{
    Light::FromJson(object);

    mConstant = object["constant"].toDouble();
    mLinear = object["linear"].toDouble();
    mQuadratic = object["quadratic"].toDouble();
}