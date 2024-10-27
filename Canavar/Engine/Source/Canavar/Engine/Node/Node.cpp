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

const QMatrix4x4& Canavar::Engine::Node::GetTransformation()
{
    if (mDirty)
    {
        Update();
    }

    return mTransformation;
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
