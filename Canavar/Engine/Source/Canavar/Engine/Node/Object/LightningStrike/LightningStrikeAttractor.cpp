#include "LightningStrikeAttractor.h"

void Canavar::Engine::LightningStrikeAttractor::ToJson(QJsonObject &object)
{
    Object::ToJson(object);
}

void Canavar::Engine::LightningStrikeAttractor::FromJson(const QJsonObject &object)
{
    Object::FromJson(object);
}

Canavar::Engine::LightningStrikeAttractor::LightningStrikeAttractor()
{
    SetNodeName("Lightning Strike Attractor");
}
