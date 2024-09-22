#include "Camera.h"

Canavar::Engine::Camera::Camera()
    : Node()
    , mActive(false)
    , mWidth(1600)
    , mHeight(900)
    , mZNear(1.0f)
    , mZFar(1000000.0f)
{
    mSelectable = false;
}

QMatrix4x4 Canavar::Engine::Camera::GetViewProjectionMatrix()
{
    return GetProjectionMatrix() * GetViewMatrix();
}

QMatrix4x4 Canavar::Engine::Camera::GetViewMatrix()
{
    QMatrix4x4 viewMatrix;
    viewMatrix.rotate(mRotation.conjugated());
    viewMatrix.translate(-mPosition);

    if (mParent)
        return viewMatrix * mParent->WorldTransformation().inverted();
    else
        return viewMatrix;
}

QMatrix4x4 Canavar::Engine::Camera::GetRotationMatrix()
{
    auto rotation = GetViewMatrix();
    rotation.setColumn(3, QVector4D(0, 0, 0, 1));
    return rotation;
}

QVector3D Canavar::Engine::Camera::GetViewDirection()
{
    return mRotation * QVector3D(0, 0, -1);
}

float Canavar::Engine::Camera::CalculateSkyYOffset(float horizonDistance)
{
    auto transform = QMatrix4x4();

    transform.setColumn(3, QVector4D(0, WorldPosition().y(), 0, 1));
    auto ndc = (GetProjectionMatrix() * transform) * QVector4D(0, 0, horizonDistance, 1);

    return ndc.y() / ndc.w();
}

bool Canavar::Engine::Camera::GetActive() const
{
    return mActive;
}

void Canavar::Engine::Camera::SetActive(bool newActive)
{
    if (mActive == newActive)
        return;
    mActive = newActive;
    emit ActiveChanged();
}

void Canavar::Engine::Camera::ToJson(QJsonObject& object)
{
    Node::ToJson(object);

    object.insert("z_near", mZNear);
    object.insert("z_far", mZFar);
}

void Canavar::Engine::Camera::FromJson(const QJsonObject& object)
{
    Node::FromJson(object);

    SetZNear(object["z_near"].toDouble());
    SetZFar(object["z_far"].toDouble());
}

void Canavar::Engine::Camera::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}