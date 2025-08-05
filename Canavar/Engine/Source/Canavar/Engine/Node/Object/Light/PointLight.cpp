#include "PointLight.h"

Canavar::Engine::PointLight::PointLight()
{
    SetNodeName("Point Light");
}

void Canavar::Engine::PointLight::ToJson(QJsonObject &object)
{
    Light::ToJson(object);

    object.insert("constant", mConstant);
    object.insert("linear", mLinear);
    object.insert("quadratic", mQuadratic);
}

void Canavar::Engine::PointLight::FromJson(const QJsonObject &object, const  QSet<NodePtr> &nodes)
{
    Light::FromJson(object, nodes);

    mConstant = object["constant"].toDouble(1.0f);
    mLinear = object["linear"].toDouble(0.05f);
    mQuadratic = object["quadratic"].toDouble(0.001f);
}
