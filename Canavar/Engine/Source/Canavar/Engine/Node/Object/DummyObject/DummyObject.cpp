#include "DummyObject.h"

Canavar::Engine::DummyObject::DummyObject()
{
    SetNodeName("Dummy Object");
}

void Canavar::Engine::DummyObject::ToJson(QJsonObject &object)
{
    Object::ToJson(object);
}

void Canavar::Engine::DummyObject::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    Object::FromJson(object, nodes);
}
