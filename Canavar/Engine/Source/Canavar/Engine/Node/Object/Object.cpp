#include "Object.h"

#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Math.h"

void Canavar::Engine::Object::SetRotation(const QQuaternion& newRotation)
{
    mRotation = newRotation;
    UpdateTransformation();
}

void Canavar::Engine::Object::SetPosition(const QVector3D& newPosition)
{
    mPosition = newPosition;
    UpdateTransformation();
}

void Canavar::Engine::Object::SetScale(const QVector3D& newScale)
{
    mScale = newScale;
    UpdateTransformation();
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

    if (const auto pParent = GetParent<Node>())
    {
        object.insert("parent_uuid", QString::fromStdString(pParent->GetUuid()));
    }

    object.insert("visible", mVisible);
    object.insert("selectable", mSelectable);
}

void Canavar::Engine::Object::FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes)
{
    Node::FromJson(object, nodes);

    QString parent_uuid = object["parent_uuid"].toString();

    if (!parent_uuid.isNull() && !parent_uuid.isEmpty())
    {
        for (const auto pNode : nodes)
        {
            if (parent_uuid == pNode->GetUuid())
            {
                SetParent(pNode);
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
