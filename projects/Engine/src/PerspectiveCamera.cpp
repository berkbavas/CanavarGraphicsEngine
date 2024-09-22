#include "PerspectiveCamera.h"
#include "Helper.h"

Canavar::Engine::PerspectiveCamera::PerspectiveCamera()
    : Camera()
{
    SetVerticalFov(60);
}

QMatrix4x4 Canavar::Engine::PerspectiveCamera::GetProjectionMatrix()
{
    QMatrix4x4 projection;
    projection.perspective(mVerticalFov, float(mWidth) / float(mHeight), mZNear, mZFar);
    return projection;
}

const float& Canavar::Engine::PerspectiveCamera::GetHorizontalFov() const
{
    return mHorizontalFov;
}

float& Canavar::Engine::PerspectiveCamera::GetHorizontalFov_NonConst()
{
    return mHorizontalFov;
}

void Canavar::Engine::PerspectiveCamera::SetHorizontalFov(float newHorizontalFov)
{
    mHorizontalFov = newHorizontalFov;
    mVerticalFov = Helper::CalculateVerticalFovForGivenHorizontalFov(mHorizontalFov, mWidth, mHeight);
}

void Canavar::Engine::PerspectiveCamera::ToJson(QJsonObject& object)
{
    Camera::ToJson(object);

    object.insert("h_fov", mHorizontalFov);
    object.insert("v_fov", mVerticalFov);
}

void Canavar::Engine::PerspectiveCamera::FromJson(const QJsonObject& object)
{
    Camera::FromJson(object);

    mHorizontalFov = object["h_fov"].toDouble();
    mVerticalFov = object["v_fov"].toDouble();
}

const float& Canavar::Engine::PerspectiveCamera::GetVerticalFov() const
{
    return mVerticalFov;
}

float& Canavar::Engine::PerspectiveCamera::GetVerticalFov_NonConst()
{
    return mVerticalFov;
}

void Canavar::Engine::PerspectiveCamera::SetVerticalFov(float newVerticalFov)
{
    mVerticalFov = newVerticalFov;
    mHorizontalFov = Helper::CalculateHorizontalFovForGivenVerticalFov(mVerticalFov, mWidth, mHeight);
}