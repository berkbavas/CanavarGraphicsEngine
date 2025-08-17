#include "Node.h"

#include "Canavar/Engine/Node/NodeVisitor.h"
#include "Canavar/Engine/Node/Object/Object.h"
#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Util.h"

Canavar::Engine::Node::Node()
{
    mUuid = Util::GenerateUuid().toStdString();
}

const std::string &Canavar::Engine::Node::GetUniqueNodeName()
{
    mUniqueNodeName = mNodeName + "##" + std::to_string(mNodeId);
    return mUniqueNodeName;
}

const std::string &Canavar::Engine::Node::GetNodeIdString()
{
    mNodeIdString = std::to_string(mNodeId);
    return mNodeIdString;
}

void Canavar::Engine::Node::ToJson(QJsonObject &object)
{
    object.insert("node_name", QString::fromStdString(mNodeName));
    object.insert("uuid", QString::fromStdString(mUuid));
    object.insert("node_type_name", QString::fromStdString(GetNodeTypeName()));
}

void Canavar::Engine::Node::FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes)
{
    mNodeName = object["node_name"].toString().toStdString();
    mUuid = object["uuid"].toString().toStdString();
}

void Canavar::Engine::Node::RemoveParent()
{
    SetParent(std::weak_ptr<Node>());
}

void Canavar::Engine::Node::SetParent(NodeWeakPtr pNewParent)
{
    LOG_DEBUG("Object::SetParent: > Setting a parent to Object at {}", static_cast<void*>(this));

    const auto pCurrentParentLocked = mParent.lock();
    const auto pNewParentLocked = pNewParent.lock();

    QVector3D worldPos;
    Object *pThisObject = nullptr;

    if (pThisObject = dynamic_cast<Object *>(this))
    {
        worldPos = pThisObject->GetWorldPosition();
    }

    if (pCurrentParentLocked == nullptr)
    {
        LOG_DEBUG("Object::SetParent: < Current parent is nullptr.");
    }

    if (pNewParentLocked == nullptr)
    {
        LOG_DEBUG("Object::SetParent: < New parent is nullptr. Reseting Weak Pointer...");
        mParent.reset();
    }

    if (pCurrentParentLocked == pNewParentLocked && pCurrentParentLocked != nullptr)
    {
        LOG_WARN("Object::SetParent: < Parent is already this object. Returning...");
        return;
    }

    if (pNewParentLocked.get() == this)
    {
        LOG_FATAL("Object::SetParent: < Cannot assign itself as a parent. Returning...");
        return;
    }

    if (pCurrentParentLocked)
    {
        pCurrentParentLocked->RemoveChild(shared_from_this());
    }

    mParent = pNewParent;

    if (mParent.lock())
    {
        mParent.lock()->AddChild(shared_from_this());
    }

    if (pThisObject)
    {
        pThisObject->SetWorldPosition(worldPos);
    }

    LOG_DEBUG("Object::SetParent: < Parent has been set. I am done.");
}

void Canavar::Engine::Node::AddChild(NodePtr pNode)
{
    LOG_DEBUG("Object::AddChild: > An Object will be added to my children list.");

    if (pNode == nullptr)
    {
        LOG_FATAL("Object::AddChild: < pNode is nullptr! Returning...");
        return;
    }

    if (pNode.get() == this)
    {
        LOG_FATAL("Object::AddChild: < Cannot add itself as a child! Returning...");
        return;
    }

    if (mChildren.contains(pNode))
    {
        LOG_WARN("Object::AddChild: < Already child of this Object. Returning...");
        return;
    }

    pNode->SetParent(shared_from_this());
    mChildren << pNode;

    LOG_DEBUG("Object::AddChild: < I am done. Returning...");
}

void Canavar::Engine::Node::RemoveChild(NodePtr pNode)
{
    mChildren.remove(pNode);
    pNode->SetParent(std::weak_ptr<Node>());
}

void Canavar::Engine::Node::RemoveChild(Node *pNode)
{
    mChildren.remove(pNode->shared_from_this());
    pNode->SetParent(std::weak_ptr<Node>());
}

const QSet<Canavar::Engine::NodePtr> &Canavar::Engine::Node::GetChildren() const
{
    return mChildren;
}
