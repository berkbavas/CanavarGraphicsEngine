#include "Node.h"

const QUuid &Canavar::Engine::Node::GetUuid() const
{
    return mUuid;
}

const std::string &Canavar::Engine::Node::GetNodeName() const
{
    return mNodeName;
}

const std::string &Canavar::Engine::Node::GetNodeUniqueName() const
{
    return mNodeUniqueName;
}

int Canavar::Engine::Node::GetNodeId() const
{
    return mNodeId;
}

void Canavar::Engine::Node::SetNodeName(const std::string &NodeName)
{
    mNodeName = NodeName;
    UpdateNodeUniqueName();
}

void Canavar::Engine::Node::UpdateNodeUniqueName()
{
    mNodeUniqueName = std::format("{}_{}", mNodeName, mNodeId);
}

void Canavar::Engine::Node::SetUuid(const QUuid &Uuid)
{
    mUuid = Uuid;
}

void Canavar::Engine::Node::SetNodeId(int NodeId)
{
    mNodeId = NodeId;
    UpdateNodeUniqueName();
}
