#include "Node.h"

#include "Canavar/Engine/Util/Logger.h"

#include <algorithm>

const QUuid& Canavar::Engine::Node::GetUuid() const
{
    return mUuid;
}

const std::string& Canavar::Engine::Node::GetNodeName() const
{
    return mNodeName;
}

const std::string& Canavar::Engine::Node::GetNodeUniqueName() const
{
    return mNodeUniqueName;
}

int Canavar::Engine::Node::GetNodeId() const
{
    return mNodeId;
}

void Canavar::Engine::Node::SetNodeName(const std::string& NodeName)
{
    mNodeName = NodeName;
    UpdateNodeUniqueName();
}

void Canavar::Engine::Node::UpdateNodeUniqueName()
{
    mNodeUniqueName = std::format("{}_{}", mNodeName, mNodeId);
}

void Canavar::Engine::Node::SetUuid(const QUuid& Uuid)
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
        LOG_WARN("Node::AddChild: Cannot add null or self as a child node.");
        return;
    }

    // Guard against cycles: pChild must not be an ancestor of this node.
    Node* pAncestor = mParent;
    while (pAncestor)
    {
        if (pAncestor == pChild)
        {
            LOG_WARN("Node::AddChild: Cannot add child node: it would create a cycle in the hierarchy.");
            return;
        }
        pAncestor = pAncestor->mParent;
    }

    if (pChild->mParent == this)
    {
        LOG_WARN("Node::AddChild: Node is already a child of this node.");
        return; // already a child
    }

    // Detach from current parent.
    if (pChild->mParent)
    {
        LOG_DEBUG("Node::AddChild: Detaching child node '{}' from its current parent '{}'", pChild->GetNodeUniqueName(), pChild->mParent->GetNodeUniqueName());
        pChild->mParent->RemoveChild(pChild);
    }

    LOG_DEBUG("Node::AddChild: Adding child node '{}' to parent '{}'", pChild->GetNodeUniqueName(), GetNodeUniqueName());
    pChild->mParent = this;
    mChildren.push_back(pChild);
}

void Canavar::Engine::Node::RemoveChild(Node* pChild)
{
    if (!pChild)
    {
        LOG_WARN("Node::RemoveChild: Cannot remove null child node.");
        return;
    }

    auto It = std::find(mChildren.begin(), mChildren.end(), pChild);
    if (It != mChildren.end())
    {
        LOG_DEBUG("Node::RemoveChild: Removing child node '{}'", pChild->GetNodeUniqueName());
        pChild->mParent = nullptr;
        mChildren.erase(It);
    }
}

void Canavar::Engine::Node::SetParent(Node* pParent)
{
    if (pParent)
    {
        LOG_DEBUG("Node::SetParent: Setting parent of node '{}' to '{}'", GetNodeUniqueName(), pParent->GetNodeUniqueName());
        pParent->AddChild(this);
    }
    else if (mParent)
    {
        LOG_DEBUG("Node::SetParent: Detaching node '{}' from its parent '{}'", GetNodeUniqueName(), mParent->GetNodeUniqueName());
        mParent->RemoveChild(this);
        // RemoveChild will set mParent to nullptr, so no need to do it here.
    }
}
