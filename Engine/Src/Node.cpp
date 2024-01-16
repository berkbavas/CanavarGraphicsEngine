#include "Node.h"

#include <QUuid>
#include <QtMath>

Canavar::Engine::Node::Node()
    : QObject()
    , mPosition(0, 0, 0)
    , mScale(1, 1, 1)
    , mParent(nullptr)
    , mVisible(true)
    , mSelectable(true)
    , mUUID()
    , mID(-1)
    , mType(NodeType::DummyNode)
    , mExcludeFromExport(false)
{
    mAABB.SetMin(QVector3D(-1.0f, -1.0f, -1.0f));
    mAABB.SetMax(QVector3D(1.0f, 1.0f, 1.0f));

    if (mUUID.isEmpty())
        mUUID = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
}

Canavar::Engine::Node::~Node() {}

Canavar::Engine::Node* Canavar::Engine::Node::FindChildByNameRecursive(const QString& name)
{
    for (const auto& child : mChildren)
        if (child->GetName() == name)
            return child;

    for (const auto& child : mChildren)
        if (Node* node = child->FindChildByNameRecursive(name))
            return node;

    return nullptr;
}

const QMatrix4x4 Canavar::Engine::Node::WorldTransformation() const
{
    // TODO: Scaling issue

    if (mParent)
    {
        // Remove scaling
        const auto ParentScale = mParent->Scale();
        mParent->SetScale(QVector3D(1.0f / ParentScale.x(), 1.0f / ParentScale.y(), 1.0f / ParentScale.z()));

        const auto WorldPosition = mParent->WorldPosition();
        auto WorldRotation = mParent->WorldRotation();

        QMatrix4x4 tr;
        tr.rotate(WorldRotation.normalized());
        tr.setColumn(3, QVector4D(WorldPosition, 1.0f));

        mParent->SetScale(ParentScale);

        return tr * mTransformation;
    }
    else
        return mTransformation;
}

void Canavar::Engine::Node::SetWorldTransformation(const QMatrix4x4& newTransformation)
{
    SetWorldPosition(mTransformation.column(3).toVector3D());
    SetWorldRotation(QQuaternion::fromRotationMatrix(mTransformation.normalMatrix()));
}

QQuaternion Canavar::Engine::Node::WorldRotation() const
{
    if (mParent)
        return mParent->WorldRotation() * mRotation;
    else
        return mRotation;
}

void Canavar::Engine::Node::SetWorldRotation(const QQuaternion& newWorldRotation)
{
    if (mParent)
        mRotation = mParent->WorldRotation().inverted() * newWorldRotation;
    else
        mRotation = newWorldRotation;

    UpdateTransformation();
}

const QQuaternion& Canavar::Engine::Node::Rotation() const
{
    return mRotation;
}

void Canavar::Engine::Node::SetRotation(const QQuaternion& newRotation)
{
    mRotation = newRotation;

    UpdateTransformation();
}

QVector3D Canavar::Engine::Node::WorldPosition() const
{
    if (mParent)
        return mParent->WorldPosition() + mParent->WorldRotation() * mPosition;
    else
        return mPosition;
}

void Canavar::Engine::Node::SetWorldPosition(const QVector3D& newWorldPosition)
{
    if (mParent)
        mPosition = mParent->WorldRotation().inverted() * (newWorldPosition - mParent->WorldPosition());
    else
        mPosition = newWorldPosition;

    UpdateTransformation();
}

const QVector3D& Canavar::Engine::Node::Position() const
{
    return mPosition;
}

void Canavar::Engine::Node::SetPosition(const QVector3D& newPosition)
{
    mPosition = newPosition;

    UpdateTransformation();
}

const QVector3D& Canavar::Engine::Node::Scale() const
{
    return mScale;
}

void Canavar::Engine::Node::SetScale(const QVector3D& newScale)
{
    mScale = newScale;

    UpdateTransformation();
}

const QMatrix4x4& Canavar::Engine::Node::Transformation() const
{
    return mTransformation;
}

void Canavar::Engine::Node::SetTransformation(const QMatrix4x4& newTransformation)
{
    mTransformation = newTransformation;

    mPosition = mTransformation.column(3).toVector3D();
    mRotation = QQuaternion::fromRotationMatrix(mTransformation.normalMatrix());
}

void Canavar::Engine::Node::UpdateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));
}

Canavar::Engine::Node* Canavar::Engine::Node::GetParent() const
{
    return mParent;
}

void Canavar::Engine::Node::SetParent(Node* newParent)
{
    // TODO: Do we need to update transformation as well?

    if (mParent == newParent)
        return;

    if (mParent)
    {
        if (IsChildOf(mParent))
            mParent->RemoveChild(this);
    }

    if (newParent)
    {
        if (!IsChildOf(newParent))
            newParent->AddChild(this);
    }

    mParent = newParent;
}

void Canavar::Engine::Node::AddChild(Node* node)
{
    // TODO: Check self assignment
    // TODO: Check if "node" is already child of this node
    // TOOO: Check if "node" has already a parent

    if (!node)
    {
        qWarning() << "Node is nullptr";
        return;
    }

    if (this == node)
    {
        qCritical() << "Cannot add a node to as a child of itself.";
        return;
    }

    if (mChildren.contains(node))
    {
        qWarning() << "Node is already a child of this node.";
        return;
    }

    if (node->GetParent())
    {
        qWarning() << "Node has already a parent.";
        return;
    }

    const auto NodeWorldPosition = node->WorldPosition();
    mChildren << node;
    node->SetParent(this);
    node->SetWorldPosition(NodeWorldPosition);
}

void Canavar::Engine::Node::RemoveChild(Node* node)
{
    if (node)
    {
        mChildren.removeAll(node);
        node->SetParent(nullptr);
    }
}

const QList<Canavar::Engine::Node*>& Canavar::Engine::Node::GetChildren() const
{
    return mChildren;
}

void Canavar::Engine::Node::ToJson(QJsonObject& object)
{
    QJsonObject rotation;
    rotation.insert("x", mRotation.x());
    rotation.insert("y", mRotation.y());
    rotation.insert("z", mRotation.z());
    rotation.insert("w", mRotation.scalar());
    object.insert("rotation", rotation);

    QJsonObject position;
    position.insert("x", mPosition.x());
    position.insert("y", mPosition.y());
    position.insert("z", mPosition.z());
    object.insert("position", position);

    QJsonObject scale;
    scale.insert("x", mScale.x());
    scale.insert("y", mScale.y());
    scale.insert("z", mScale.z());
    object.insert("scale", scale);

    if (mParent)
        object.insert("parent", mParent->GetUUID());

    object.insert("visible", mVisible);
    object.insert("name", mName);
    object.insert("uuid", mUUID);
    object.insert("type", (int)mType);
    object.insert("selectable", mSelectable);
}

void Canavar::Engine::Node::FromJson(const QJsonObject& object)
{
    // Rotation
    {
        float x = object["rotation"]["x"].toDouble();
        float y = object["rotation"]["y"].toDouble();
        float z = object["rotation"]["z"].toDouble();
        float w = object["rotation"]["w"].toDouble();

        SetRotation(QQuaternion(w, x, y, z));
    }

    // Position
    {
        float x = object["position"]["x"].toDouble();
        float y = object["position"]["y"].toDouble();
        float z = object["position"]["z"].toDouble();

        SetPosition(QVector3D(x, y, z));
    }

    // Scale
    {
        float x = object["scale"]["x"].toDouble();
        float y = object["scale"]["y"].toDouble();
        float z = object["scale"]["z"].toDouble();

        SetScale(QVector3D(x, y, z));
    }

    mVisible = object["visible"].toBool();
    mName = object["name"].toString();
    mUUID = object["uuid"].toString();
    mSelectable = object["selectable"].toBool();
}

bool Canavar::Engine::Node::IsChildOf(Node* node)
{
    if (node)
        for (const auto& child : node->GetChildren())
            if (child == this)
                return true;

    return false;
}

bool Canavar::Engine::Node::IsParentOf(Node* node)
{
    if (node)
        for (const auto& child : mChildren)
            if (child == node)
                return true;

    return false;
}