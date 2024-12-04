#include "LightningStrikeAttractor.h"

void Canavar::Engine::LightningStrikeAttractor::ToJson(QJsonObject &object)
{
    Object::ToJson(object);
}

void Canavar::Engine::LightningStrikeAttractor::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    Object::FromJson(object, nodes);
}

Canavar::Engine::LightningStrikeAttractor::LightningStrikeAttractor()
{
    SetNodeName("Lightning Strike Attractor");
}
