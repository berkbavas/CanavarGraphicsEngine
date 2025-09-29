#include "Object.h"

#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Math.h"

void Canavar::Engine::Object::SetRotation(const QQuaternion& NewRotation)
{
    mRotation = NewRotation;
    UpdateTransformation();
}

void Canavar::Engine::Object::SetPosition(const QVector3D& NewPosition)
{
    mPosition = NewPosition;
    UpdateTransformation();
}

void Canavar::Engine::Object::SetScale(const QVector3D& NewScale)
{
    mScale = NewScale;
    UpdateTransformation();
}

void Canavar::Engine::Object::ToJson(QJsonObject& Object)
{
    Node::ToJson(Object);

    QJsonObject Rotation;
    Rotation.insert("x", mRotation.x());
    Rotation.insert("y", mRotation.y());
    Rotation.insert("z", mRotation.z());
    Rotation.insert("w", mRotation.scalar());
    Object.insert("rotation", Rotation);

    QJsonObject Position;
    Position.insert("x", mPosition.x());
    Position.insert("y", mPosition.y());
    Position.insert("z", mPosition.z());
    Object.insert("position", Position);

    QJsonObject Scale;
    Scale.insert("x", mScale.x());
    Scale.insert("y", mScale.y());
    Scale.insert("z", mScale.z());
    Object.insert("scale", Scale);

    if (const auto pParent = GetParent<Node>())
    {
        Object.insert("parent_uuid", QString::fromStdString(pParent->GetUuid()));
    }

    Object.insert("visible", mVisible);
    Object.insert("selectable", mSelectable);
}

void Canavar::Engine::Object::FromJson(const QJsonObject& Object, const QSet<NodePtr>& Nodes)
{
    Node::FromJson(Object, Nodes);

    QString ParentUuid = Object["parent_uuid"].toString();

    if (!ParentUuid.isNull() && !ParentUuid.isEmpty())
    {
        for (const auto pNode : Nodes)
        {
            if (ParentUuid == pNode->GetUuid())
            {
                SetParent(pNode);
            }
        }
    }

    // Rotation
    {
        float x = Object["rotation"]["x"].toDouble();
        float y = Object["rotation"]["y"].toDouble();
        float z = Object["rotation"]["z"].toDouble();
        float w = Object["rotation"]["w"].toDouble(1.0f);

        SetRotation(QQuaternion(w, x, y, z));
    }

    // Position
    {
        float x = Object["position"]["x"].toDouble();
        float y = Object["position"]["y"].toDouble();
        float z = Object["position"]["z"].toDouble();

        SetPosition(QVector3D(x, y, z));
    }

    // Scale
    {
        float x = Object["scale"]["x"].toDouble(1.0f);
        float y = Object["scale"]["y"].toDouble(1.0f);
        float z = Object["scale"]["z"].toDouble(1.0f);

        SetScale(QVector3D(x, y, z));
    }

    mVisible = Object["visible"].toBool(true);
    mSelectable = Object["selectable"].toBool(true);

    UpdateTransformation();
    Canavar::Engine::Math::ConstructFromEulerDegrees(mYaw, mPitch, mRoll);
}

QMatrix4x4 Canavar::Engine::Object::GetWorldTransformation() const
{
    QMatrix4x4 WorldTransformation;

    if (const auto pParent = GetParent<Object>())
    {
        WorldTransformation.rotate(pParent->GetWorldRotation().normalized());
        WorldTransformation.setColumn(3, QVector4D(pParent->GetWorldPosition(), 1.0f));

        WorldTransformation = WorldTransformation * GetTransformation();
    }
    else
    {
        WorldTransformation.scale(mScale);
        WorldTransformation.rotate(GetWorldRotation());
        WorldTransformation.setColumn(3, QVector4D(GetWorldPosition(), 1.0f));
    }

    return WorldTransformation;
}

QQuaternion Canavar::Engine::Object::GetWorldRotation() const
{
    if (const auto pParent = GetParent<Object>())
    {
        return pParent->GetWorldRotation() * mRotation;
    }
    else
    {
        return mRotation;
    }
}

QVector3D Canavar::Engine::Object::GetWorldPosition() const
{
    if (const auto pParent = GetParent<Object>())
    {
        return pParent->GetWorldPosition() + pParent->GetWorldRotation() * mPosition;
    }
    else
    {
        return mPosition;
    }
}

void Canavar::Engine::Object::SetWorldRotation(const QQuaternion& NewRotation)
{
    if (const auto pParent = GetParent<Object>())
    {
        SetRotation(pParent->GetWorldRotation().inverted() * NewRotation);
    }
    else
    {
        SetRotation(NewRotation);
    }
}

void Canavar::Engine::Object::SetWorldPosition(const QVector3D& NewPosition)
{
    if (const auto pParent = GetParent<Object>())
    {
        SetPosition(pParent->GetWorldRotation().inverted() * (NewPosition - pParent->GetWorldPosition()));
    }
    else
    {
        SetPosition(NewPosition);
    }
}

void Canavar::Engine::Object::SetWorldPosition(float x, float y, float z)
{
    SetWorldPosition(QVector3D(x, y, z));
}
void Canavar::Engine::Object::SetTransformation(const QMatrix4x4& NewTransformation)
{
    mTransformation = NewTransformation;
    mPosition = mTransformation.column(3).toVector3D();
    mRotation = QQuaternion::fromRotationMatrix(mTransformation.normalMatrix());
}

void Canavar::Engine::Object::SetPosition(float x, float y, float z)
{
    SetPosition(QVector3D(x, y, z));
}

void Canavar::Engine::Object::SetScale(float x, float y, float z)
{
    SetScale(QVector3D(x, y, z));
}

void Canavar::Engine::Object::SetScale(float uniformScale)
{
    SetScale(uniformScale, uniformScale, uniformScale);
}

void Canavar::Engine::Object::RotateGlobal(const QVector3D& Axis, float Angle)
{
    SetWorldRotation(QQuaternion::fromAxisAndAngle(Axis, Angle) * GetWorldRotation());
}

void Canavar::Engine::Object::RotateLocal(const QVector3D& Axis, float Angle)
{
    SetWorldRotation(GetWorldRotation() * QQuaternion::fromAxisAndAngle(Axis, Angle));
}

void Canavar::Engine::Object::Translate(const QVector3D& Delta)
{
    SetWorldPosition(GetWorldPosition() + Delta);
}

void Canavar::Engine::Object::UpdateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));

    mNormalMatrix = mTransformation.normalMatrix();
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

void Canavar::Engine::Object::SetYaw(float Yaw)
{
    mYaw = Yaw;
    SetRotation(Canavar::Engine::Math::ConstructFromEulerDegrees(mYaw, mPitch, mRoll));
}

void Canavar::Engine::Object::SetPitch(float Pitch)
{
    mPitch = Pitch;
    SetRotation(Canavar::Engine::Math::ConstructFromEulerDegrees(mYaw, mPitch, mRoll));
}

void Canavar::Engine::Object::SetRoll(float Roll)
{
    mRoll = Roll;
    SetRotation(Canavar::Engine::Math::ConstructFromEulerDegrees(mYaw, mPitch, mRoll));
}
