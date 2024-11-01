#include "Node.h"

#include "Canavar/Engine/Util/Util.h"

Canavar::Engine::Node::Node()
{
    mUuid = Util::GenerateUuid();
}

Canavar::Engine::Node::Node(const QString &uuid)
    : mUuid(uuid)
{}

void Canavar::Engine::Node::ToJson(QJsonObject &object)
{
    object.insert("node_name", mNodeName);
    object.insert("uuid", mUuid);
    object.insert("node_type", QString(GetNodeType()));
}

void Canavar::Engine::Node::FromJson(const QJsonObject &object)
{
    mNodeName = object["node_name"].toString();
}

QString Canavar::Engine::Node::GetUniqueNodeName() const
{
    return mNodeName + "##" + QString::number(mNodeId);
}
