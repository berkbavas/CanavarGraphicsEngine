#include "PerspectiveCamera.h"

QMatrix4x4 Canavar::Engine::PerspectiveCamera::GetProjectionMatrix() const
{
    QMatrix4x4 ProjectionMatrix;
    ProjectionMatrix.perspective(mVerticalFov, GetAspectRatio(), GetZNear(), GetZFar());
    return ProjectionMatrix;
}

QMatrix4x4 Canavar::Engine::PerspectiveCamera::GetViewProjectionMatrix() const
{
    return GetProjectionMatrix() * GetViewMatrix();
}

QMatrix4x4 Canavar::Engine::PerspectiveCamera::GetViewMatrix() const
{
    QMatrix4x4 ViewMatrix;
    ViewMatrix.rotate(GetWorldRotation().conjugated());
    ViewMatrix.translate(-GetWorldPosition());
    return ViewMatrix;
}

QMatrix4x4 Canavar::Engine::PerspectiveCamera::GetRotationMatrix() const
{
    constexpr QVector4D ZERO_TRANSLATION(0, 0, 0, 1);

    QMatrix4x4 ViewMatrix = GetViewMatrix();
    ViewMatrix.setColumn(3, ZERO_TRANSLATION);
    return ViewMatrix;
}

QVector3D Canavar::Engine::PerspectiveCamera::GetViewDirection() const
{
    constexpr QVector3D NEGATIVE_Z(0, 0, -1);
    return GetWorldRotation() * NEGATIVE_Z;
}

float Canavar::Engine::PerspectiveCamera::GetHorizontalFov() const
{
    const auto HFOV = std::atan(std::tan(qDegreesToRadians(mVerticalFov) / 2.0f) * GetAspectRatio()) * 2.0f;

    return std::abs(qRadiansToDegrees(HFOV));
}

float Canavar::Engine::PerspectiveCamera::GetAspectRatio() const
{
    return float(mWidth) / float(mHeight);
}

void Canavar::Engine::PerspectiveCamera::ToJson(QJsonObject& object)
{
    Camera::ToJson(object);

    object.insert("z_near", mZNear);
    object.insert("z_far", mZFar);
    object.insert("vertical_fov", mVerticalFov);
}

void Canavar::Engine::PerspectiveCamera::FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes)
{
    Camera::FromJson(object, nodes);

    mZNear = object["z_near"].toDouble(1.0f);
    mZFar = object["z_far"].toDouble(100'000.0f);
    mVerticalFov = object["vertical_fov"].toDouble(60.0f);
}
