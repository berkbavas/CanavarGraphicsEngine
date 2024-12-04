#include "Node.h"

#include "Canavar/Engine/Util/Util.h"

Canavar::Engine::Node::Node()
{
    mUuid = Util::GenerateUuid();
}

void Canavar::Engine::Node::ToJson(QJsonObject &object)
{
    object.insert("node_name", mNodeName);
    object.insert("uuid", mUuid);
    object.insert("node_type", GetNodeType());
}

void Canavar::Engine::Node::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    mNodeName = object["node_name"].toString();
    mUuid = object["uuid"].toString();
}

QString Canavar::Engine::Node::GetUniqueNodeName() const
{
    return mNodeName + "##" + QString::number(mNodeId);
}
