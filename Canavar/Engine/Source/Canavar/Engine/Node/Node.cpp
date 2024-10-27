#include "Node.h"

#include "Canavar/Engine/Util/Math.h"

Canavar::Engine::Node::Node()
{
    UpdateRotationFromEulerDegrees();
}

void Canavar::Engine::Node::SetRotation(const QQuaternion& newRotation)
{
    if (mRotation == newRotation)
    {
        return;
    }

    mRotation = newRotation;

    if (!mUpdateFromEulerAngles)
    {
        Canavar::Engine::Math::GetEulerDegrees(mRotation, mYaw, mPitch, mRoll);
    }

    mUpdateFromEulerAngles = false;
    MakeDirty();
}

void Canavar::Engine::Node::SetPosition(const QVector3D& newPosition)
{
    if (mPosition == newPosition)
    {
        return;
    }

    mPosition = newPosition;
    MakeDirty();
}

void Canavar::Engine::Node::SetScale(const QVector3D& newScale)
{
    if (mScale == newScale)
    {
        return;
    }

    mScale = newScale;
    MakeDirty();
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
        mWorldTransformation = GetTransformation();
    }

    return mWorldTransformation;
}

const QMatrix4x4& Canavar::Engine::Node::GetTransformation()
{
    if (mDirty)
    {
        Update();
    }

    return mTransformation;
}

const QQuaternion& Canavar::Engine::Node::GetWorldRotation()
{
    if (const auto pParent = GetParent())
    {
        mWorldRotation = pParent->GetWorldRotation() * mRotation;
        return mWorldRotation;
    }

    return mRotation;
}

const QVector3D& Canavar::Engine::Node::GetWorldPosition()
{
    if (const auto pParent = GetParent())
    {
        mWorldPosition = pParent->GetWorldPosition() + pParent->GetWorldRotation() * mPosition;
        return mWorldPosition;
    }

    return mPosition;
}

void Canavar::Engine::Node::SetWorldRotation(const QQuaternion& newRotation)
{
    if (const auto pParent = GetParent())
    {
        SetRotation(pParent->GetWorldRotation().inverted() * newRotation);
    }
    else
    {
        SetRotation(newRotation);
    }
}

void Canavar::Engine::Node::SetWorldPosition(const QVector3D& newPosition)
{
    if (const auto pParent = GetParent())
    {
        SetPosition(pParent->GetWorldRotation().inverted() * (newPosition - pParent->GetWorldPosition()));
    }
    else
    {
        SetPosition(newPosition);
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
    MakeDirty();
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
    SetRotation(QQuaternion::fromAxisAndAngle(axis, angle) * GetRotation());
}

void Canavar::Engine::Node::RotateLocal(const QVector3D& axis, float angle)
{
    SetRotation(GetRotation() * QQuaternion::fromAxisAndAngle(axis, angle));
}

void Canavar::Engine::Node::Translate(const QVector3D& delta)
{
    SetPosition(mPosition + delta);
}

void Canavar::Engine::Node::Update()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));

    mNormalMatrix = mTransformation.normalMatrix();
    mDirty = false;
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
