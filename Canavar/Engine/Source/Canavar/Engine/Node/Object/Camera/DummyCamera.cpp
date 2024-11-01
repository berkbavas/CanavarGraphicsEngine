#include "DummyCamera.h"

Canavar::Engine::DummyCamera::DummyCamera()
{
    SetNodeName("Dummy Camera");
}

void Canavar::Engine::DummyCamera::ToJson(QJsonObject &object)
{
    PerspectiveCamera::ToJson(object);
}

void Canavar::Engine::DummyCamera::FromJson(const QJsonObject &object)
{
    PerspectiveCamera::FromJson(object);
}
