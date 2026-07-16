#include "Node.h"

#include <algorithm>

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

// ─────────────────────────────────────────────────────────────────────────────
// Hierarchy
// ─────────────────────────────────────────────────────────────────────────────

Canavar::Engine::Node* Canavar::Engine::Node::GetParent() const
{
    return mParent;
}

const std::vector<Canavar::Engine::Node*>& Canavar::Engine::Node::GetChildren() const
{
    return mChildren;
}

void Canavar::Engine::Node::AddChild(Node* pChild)
{
    if (!pChild || pChild == this)
    {
        return;
    }

    // Guard against cycles: pChild must not be an ancestor of this node.
    Node* pAncestor = mParent;
    while (pAncestor)
    {
        if (pAncestor == pChild)
        {
            return;
        }
        pAncestor = pAncestor->mParent;
    }

    if (pChild->mParent == this)
    {
        return; // already a child
    }

    // Detach from current parent.
    if (pChild->mParent)
    {
        pChild->mParent->RemoveChild(pChild);
    }

    pChild->mParent = this;
    mChildren.push_back(pChild);
}

void Canavar::Engine::Node::RemoveChild(Node* pChild)
{
    if (!pChild)
    {
        return;
    }

    auto It = std::find(mChildren.begin(), mChildren.end(), pChild);
    if (It != mChildren.end())
    {
        pChild->mParent = nullptr;
        mChildren.erase(It);
    }
}

void Canavar::Engine::Node::SetParent(Node* pParent)
{
    if (pParent)
    {
        pParent->AddChild(this);
    }
    else if (mParent)
    {
        mParent->RemoveChild(this);
    }
}
