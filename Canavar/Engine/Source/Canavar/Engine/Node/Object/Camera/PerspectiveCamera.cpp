#include "PerspectiveCamera.h"

const QMatrix4x4& Canavar::Engine::PerspectiveCamera::GetProjectionMatrix()
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(mVerticalFov, GetAspectRatio(), GetZNear(), GetZFar());
    return mProjectionMatrix;
}

const QMatrix4x4& Canavar::Engine::PerspectiveCamera::GetViewProjectionMatrix()
{
    mViewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();
    return mViewProjectionMatrix;
}

const QMatrix4x4& Canavar::Engine::PerspectiveCamera::GetViewMatrix()
{
    mViewMatrix.setToIdentity();
    mViewMatrix.rotate(GetWorldRotation().conjugated());
    mViewMatrix.translate(-GetWorldPosition());
    return mViewMatrix;
}

const QMatrix4x4& Canavar::Engine::PerspectiveCamera::GetRotationMatrix()
{
    constexpr QVector4D ZERO_TRANSLATION(0, 0, 0, 1);
    mRotationMatrix = GetViewMatrix();
    mRotationMatrix.setColumn(3, ZERO_TRANSLATION);
    return mRotationMatrix;
}

const QVector3D& Canavar::Engine::PerspectiveCamera::GetViewDirection()
{
    constexpr QVector3D NEGATIVE_Z(0, 0, -1);
    mViewDirection = GetWorldRotation() * NEGATIVE_Z;
    return mViewDirection;
}

void Canavar::Engine::PerspectiveCamera::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

float Canavar::Engine::PerspectiveCamera::GetHorizontalFov() const
{
    const auto hfov = std::atan(std::tan(qDegreesToRadians(mVerticalFov) / 2.0f) * GetAspectRatio()) * 2.0f;

    return std::abs(qRadiansToDegrees(hfov));
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
