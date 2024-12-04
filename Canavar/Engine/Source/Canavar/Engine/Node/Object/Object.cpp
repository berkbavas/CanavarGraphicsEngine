#include "Object.h"

#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Math.h"

void Canavar::Engine::Object::SetRotation(const QQuaternion& newRotation)
{
    mRotation = newRotation;

    if (const auto pParent = GetParent())
    {
        mWorldRotation = pParent->GetWorldRotation() * mRotation;
    }
    else
    {
        mWorldRotation = mRotation;
    }

    MakeTransformationDirty();
}

void Canavar::Engine::Object::SetPosition(const QVector3D& newPosition)
{
    mPosition = newPosition;

    if (const auto pParent = GetParent())
    {
        mWorldPosition = pParent->GetWorldPosition() + pParent->GetWorldRotation() * mPosition;
    }
    else
    {
        mWorldPosition = mPosition;
    }

    MakeTransformationDirty();
}

void Canavar::Engine::Object::SetScale(const QVector3D& newScale)
{
    if (mScale == newScale)
    {
        return;
    }

    mScale = newScale;
    MakeTransformationDirty();
}

void Canavar::Engine::Object::ToJson(QJsonObject& object)
{
    Node::ToJson(object);

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

    if (GetParent())
    {
        object.insert("parent", GetParent()->GetUuid());
    }

    object.insert("visible", mVisible);
    object.insert("selectable", mSelectable);

    // TODO : AABB
}

void Canavar::Engine::Object::FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes)
{
    Node::FromJson(object, nodes);

    QString parent = object["parent"].toString();

    if (!parent.isNull() && !parent.isEmpty())
    {
        if (const auto it = nodes.find(parent); it != nodes.end())
        {
            if (const auto pParent = std::dynamic_pointer_cast<Object>(it->second))
            {
                SetParent(pParent);
            }
        }
    }

    // Rotation
    {
        float x = object["rotation"]["x"].toDouble();
        float y = object["rotation"]["y"].toDouble();
        float z = object["rotation"]["z"].toDouble();
        float w = object["rotation"]["w"].toDouble(1.0f);

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
        float x = object["scale"]["x"].toDouble(1.0f);
        float y = object["scale"]["y"].toDouble(1.0f);
        float z = object["scale"]["z"].toDouble(1.0f);

        SetScale(QVector3D(x, y, z));
    }

    mVisible = object["visible"].toBool(true);
    mSelectable = object["selectable"].toBool(true);

    UpdateTransformation();
    Canavar::Engine::Math::ConstructFromEulerDegrees(mYaw, mPitch, mRoll);
}

const QMatrix4x4& Canavar::Engine::Object::GetWorldTransformation()
{
    mWorldTransformation.setToIdentity();

    if (const auto pParent = GetParent())
    {
        const auto WorldPosition = pParent->GetWorldPosition();
        const auto WorldRotation = pParent->GetWorldRotation();

        mWorldTransformation.rotate(WorldRotation.normalized());
        mWorldTransformation.setColumn(3, QVector4D(WorldPosition, 1.0f));

        mWorldTransformation = mWorldTransformation * GetTransformation();
    }
    else
    {
        mWorldTransformation.setToIdentity();
        mWorldTransformation.scale(mScale);
        mWorldTransformation.rotate(mWorldRotation);
        mWorldTransformation.setColumn(3, QVector4D(mWorldPosition, 1.0f));
    }

    return mWorldTransformation;
}

const QMatrix4x4& Canavar::Engine::Object::GetTransformation()
{
    if (mTransformationDirty)
    {
        UpdateTransformation();
    }

    return mTransformation;
}

const QQuaternion& Canavar::Engine::Object::GetWorldRotation()
{
    if (const auto pParent = GetParent())
    {
        mWorldRotation = pParent->GetWorldRotation() * mRotation;
    }
    else
    {
        mWorldRotation = mRotation;
    }

    return mWorldRotation;
}

const QVector3D& Canavar::Engine::Object::GetWorldPosition()
{
    if (const auto pParent = GetParent())
    {
        mWorldPosition = pParent->GetWorldPosition() + pParent->GetWorldRotation() * mPosition;
    }
    else
    {
        mWorldPosition = mPosition;
    }

    return mWorldPosition;
}

void Canavar::Engine::Object::SetWorldRotation(const QQuaternion& newRotation)
{
    mWorldRotation = newRotation;

    if (const auto pParent = GetParent())
    {
        SetRotation(pParent->GetWorldRotation().inverted() * mWorldRotation);
    }
    else
    {
        SetRotation(mWorldRotation);
    }
}

void Canavar::Engine::Object::SetWorldPosition(const QVector3D& newPosition)
{
    mWorldPosition = newPosition;

    if (const auto pParent = GetParent())
    {
        SetPosition(pParent->GetWorldRotation().inverted() * (mWorldPosition - pParent->GetWorldPosition()));
    }
    else
    {
        SetPosition(mWorldPosition);
    }
}

void Canavar::Engine::Object::SetWorldPosition(float x, float y, float z)
{
    SetWorldPosition(QVector3D(x, y, z));
}
void Canavar::Engine::Object::SetTransformation(const QMatrix4x4& newTransformation)
{
    if (mTransformation == newTransformation)
    {
        return;
    }

    mTransformation = newTransformation;
    mPosition = mTransformation.column(3).toVector3D();
    mRotation = QQuaternion::fromRotationMatrix(mTransformation.normalMatrix());
    MakeTransformationDirty();
}

void Canavar::Engine::Object::SetPosition(float x, float y, float z)
{
    SetPosition(QVector3D(x, y, z));
}

void Canavar::Engine::Object::SetScale(float x, float y, float z)
{
    SetScale(QVector3D(x, y, z));
}

void Canavar::Engine::Object::RotateGlobal(const QVector3D& axis, float angle)
{
    SetWorldRotation(QQuaternion::fromAxisAndAngle(axis, angle) * GetWorldRotation());
}

void Canavar::Engine::Object::RotateLocal(const QVector3D& axis, float angle)
{
    SetWorldRotation(GetWorldRotation() * QQuaternion::fromAxisAndAngle(axis, angle));
}

void Canavar::Engine::Object::Translate(const QVector3D& delta)
{
    SetWorldPosition(GetWorldPosition() + delta);
}

void Canavar::Engine::Object::UpdateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));

    mNormalMatrix = mTransformation.normalMatrix();
    mTransformationDirty = false;
}

float& Canavar::Engine::Object::GetYaw()
{
    return mYaw;
}

float& Canavar::Engine::Object::GetPitch()
{
    return mPitch;
}

float& Canavar::Engine::Object::GetRoll()
{
    return mRoll;
}

void Canavar::Engine::Object::SetYaw(float yaw)
{
    mYaw = yaw;
    SetRotation(Canavar::Engine::Math::ConstructFromEulerDegrees(mYaw, mPitch, mRoll));
}

void Canavar::Engine::Object::SetPitch(float pitch)
{
    mPitch = pitch;
    SetRotation(Canavar::Engine::Math::ConstructFromEulerDegrees(mYaw, mPitch, mRoll));
}

void Canavar::Engine::Object::SetRoll(float roll)
{
    mRoll = roll;
    SetRotation(Canavar::Engine::Math::ConstructFromEulerDegrees(mYaw, mPitch, mRoll));
}

Canavar::Engine::ObjectPtr Canavar::Engine::Object::GetParent() const
{
    return mParent.lock();
}

void Canavar::Engine::Object::RemoveParent()
{
    SetParent(std::weak_ptr<Object>());
}

void Canavar::Engine::Object::SetParent(ObjectWeakPtr pNewParent)
{
    LOG_DEBUG("Object::SetParent: > Setting a parent to Object at {}", PRINT_ADDRESS(this));

    const auto worldPos = GetWorldPosition();

    const auto pCurrentParentLocked = mParent.lock();
    const auto pNewParentLocked = pNewParent.lock();

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

    SetWorldPosition(worldPos);

    LOG_DEBUG("Object::SetParent: < Parent has been set. I am done.");
}

void Canavar::Engine::Object::AddChild(ObjectPtr pNode)
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
    mChildren.emplace(pNode);

    LOG_DEBUG("Object::AddChild: < I am done. Returning...");
}

void Canavar::Engine::Object::RemoveChild(ObjectPtr pNode)
{
    mChildren.extract(pNode);
    pNode->SetParent(std::weak_ptr<Object>());
}
