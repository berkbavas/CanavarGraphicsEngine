#include "PointLight.h"

void Canavar::Engine::PointLight::ToJson(QJsonObject &Object)
{
    Light::ToJson(Object);

    Object.insert("constant", mConstant);
    Object.insert("linear", mLinear);
    Object.insert("quadratic", mQuadratic);
}

void Canavar::Engine::PointLight::FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes)
{
    Light::FromJson(Object, Nodes);

    mConstant = Object["constant"].toDouble(1.0f);
    mLinear = Object["linear"].toDouble(0.05f);
    mQuadratic = Object["quadratic"].toDouble(0.001f);
}
