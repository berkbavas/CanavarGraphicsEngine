#include "DummyCamera.h"

Canavar::Engine::DummyCamera::DummyCamera()
{
    SetNodeName("Dummy Camera");
}

void Canavar::Engine::DummyCamera::ToJson(QJsonObject &object)
{
    PerspectiveCamera::ToJson(object);
}

void Canavar::Engine::DummyCamera::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    PerspectiveCamera::FromJson(object, nodes);
}
