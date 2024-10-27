#include "PerspectiveCamera.h"

const QMatrix4x4& Canavar::Engine::PerspectiveCamera::GetProjectionMatrix()
{
    UpdateProjectionIfDirty();
    return mProjectionMatrix;
}

const QMatrix4x4& Canavar::Engine::PerspectiveCamera::GetViewProjectionMatrix()
{
    UpdateCacheIfDirty();
    return mViewProjectionMatrix;
}

const QMatrix4x4& Canavar::Engine::PerspectiveCamera::GetViewMatrix()
{
    UpdateCacheIfDirty();
    return mViewMatrix;
}

const QMatrix4x4& Canavar::Engine::PerspectiveCamera::GetRotationMatrix()
{
    UpdateCacheIfDirty();
    return mRotationMatrix;
}

const QVector3D& Canavar::Engine::PerspectiveCamera::GetViewDirection()
{
    UpdateCacheIfDirty();
    return mViewDirection;
}

void Canavar::Engine::PerspectiveCamera::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
    UpdateProjection();
    UpdateCache();
}

float Canavar::Engine::PerspectiveCamera::GetHorizontalFov() const
{
    const auto hfov = std::atan(std::atan(mVerticalFov / 2.0) / GetAspectRatio()) * 2.0f;

    return std::abs(qRadiansToDegrees(hfov));
}

float Canavar::Engine::PerspectiveCamera::GetAspectRatio() const
{
    return float(mWidth) / float(mHeight);
}

void Canavar::Engine::PerspectiveCamera::UpdateProjection()
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(mVerticalFov, GetAspectRatio(), GetZNear(), GetZFar());
    mIsProjectionDirty = false;
}

void Canavar::Engine::PerspectiveCamera::UpdateCache()
{
    mViewMatrix.setToIdentity();
    mViewMatrix.rotate(GetRotation().conjugated());
    mViewMatrix.translate(-GetPosition());

    constexpr QVector4D ZERO_TRANSLATION(0, 0, 0, 1);
    mRotationMatrix = mViewMatrix;
    mRotationMatrix.setColumn(3, ZERO_TRANSLATION);

    constexpr QVector3D NEGATIVE_Z(0, 0, -1);
    mViewDirection = GetRotation() * NEGATIVE_Z;

    UpdateProjectionIfDirty();
    mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;

    MakeClean();
}

void Canavar::Engine::PerspectiveCamera::UpdateProjectionIfDirty()
{
    if (mIsProjectionDirty)
    {
        UpdateProjection();
    }
}

void Canavar::Engine::PerspectiveCamera::UpdateCacheIfDirty()
{
    if (GetDirty())
    {
        UpdateCache();
    }
}
