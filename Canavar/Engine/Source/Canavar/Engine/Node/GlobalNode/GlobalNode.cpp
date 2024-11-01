#include "GlobalNode.h"

void Canavar::Engine::GlobalNode::ToJson(QJsonObject &object)
{
    Node::ToJson(object);

    object.insert("enabled", mEnabled);
}

void Canavar::Engine::GlobalNode::FromJson(const QJsonObject &object)
{
    Node::FromJson(object);

    mEnabled = object["enabled"].toBool(true);
}

void Canavar::Engine::GlobalNode::Initialize() {}