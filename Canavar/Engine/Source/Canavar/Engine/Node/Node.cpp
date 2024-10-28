#include "Node.h"

#include "Canavar/Engine/Util/Math.h"

Canavar::Engine::Node::Node()
{
    UpdateRotationFromEulerDegrees();
}

void Canavar::Engine::Node::SetRotation(const QQuaternion& newRotation)
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

    if (!mUpdateFromEulerAngles)
    {
        Canavar::Engine::Math::GetEulerDegrees(mRotation, mYaw, mPitch, mRoll);
    }

    mUpdateFromEulerAngles = false;

    MakeTransformationDirty();
}

void Canavar::Engine::Node::SetPosition(const QVector3D& newPosition)
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

void Canavar::Engine::Node::SetScale(const QVector3D& newScale)
{
    if (mScale == newScale)
    {
        return;
    }

    mScale = newScale;
    MakeTransformationDirty();
}

const QMatrix4x4& Canavar::Engine::Node::GetWorldTransformation()
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

const QMatrix4x4& Canavar::Engine::Node::GetTransformation()
{
    if (mTransformationDirty)
    {
        UpdateTransformation();
    }

    return mTransformation;
}

const QQuaternion& Canavar::Engine::Node::GetWorldRotation()
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

const QVector3D& Canavar::Engine::Node::GetWorldPosition()
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

void Canavar::Engine::Node::SetWorldRotation(const QQuaternion& newRotation)
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

void Canavar::Engine::Node::SetWorldPosition(const QVector3D& newPosition)
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

void Canavar::Engine::Node::SetWorldPosition(float x, float y, float z)
{
    SetWorldPosition(QVector3D(x, y, z));
}
void Canavar::Engine::Node::SetTransformation(const QMatrix4x4& newTransformation)
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

void Canavar::Engine::Node::SetPosition(float x, float y, float z)
{
    SetPosition(QVector3D(x, y, z));
}

void Canavar::Engine::Node::SetScale(float x, float y, float z)
{
    SetScale(QVector3D(x, y, z));
}

void Canavar::Engine::Node::RotateGlobal(const QVector3D& axis, float angle)
{
    SetWorldRotation(QQuaternion::fromAxisAndAngle(axis, angle) * GetWorldRotation());
}

void Canavar::Engine::Node::RotateLocal(const QVector3D& axis, float angle)
{
    SetWorldRotation(GetWorldRotation() * QQuaternion::fromAxisAndAngle(axis, angle));
}

void Canavar::Engine::Node::Translate(const QVector3D& delta)
{
    SetWorldPosition(GetWorldPosition() + delta);
}

void Canavar::Engine::Node::UpdateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));

    mNormalMatrix = mTransformation.normalMatrix();
    mTransformationDirty = false;
}

void Canavar::Engine::Node::UpdateRotationFromEulerDegrees()
{
    mUpdateFromEulerAngles = true;
    SetRotation(Canavar::Engine::Math::ConstructFromEulerDegrees(mYaw, mPitch, mRoll));
}

float& Canavar::Engine::Node::GetYaw()
{
    return mYaw;
}

float& Canavar::Engine::Node::GetPitch()
{
    return mPitch;
}

float& Canavar::Engine::Node::GetRoll()
{
    return mRoll;
}

Canavar::Engine::NodePtr Canavar::Engine::Node::GetParent()
{
    return mParent.lock();
}

void Canavar::Engine::Node::SetParent(NodeWeakPtr pParentNode)
{
    // TODO: Log

    const auto pParent = mParent.lock();

    if (pParent == pParentNode.lock())
    {
        return;
    }

    if (pParent)
    {
        pParent->RemoveChild(shared_from_this());
    }

    mParent = pParentNode;

    if (mParent.lock())
    {
        mParent.lock()->AddChild(shared_from_this());
    }
}

void Canavar::Engine::Node::AddChild(NodePtr pNode)
{
    // TODO: Log
    if (pNode == nullptr)
    {
        return;
    }

    if (pNode.get() == this)
    {
        // Cannot assign to itself..
        return;
    }

    if (pNode->GetParent().get() == this)
    {
        // pNode is a child of this node already.
        return;
    }

    pNode->SetParent(shared_from_this());
    mChildren.emplace(pNode);
}

void Canavar::Engine::Node::RemoveChild(NodePtr pNode)
{
    mChildren.extract(pNode);
    pNode->SetParent(std::weak_ptr<Node>()); // Empty Node Ptr
}
