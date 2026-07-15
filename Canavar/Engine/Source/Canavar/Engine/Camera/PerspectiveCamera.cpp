#include "PerspectiveCamera.h"

QMatrix4x4 Canavar::Engine::PerspectiveCamera::GetProjectionMatrix() const
{
    QMatrix4x4 ProjectionMatrix;
    ProjectionMatrix.perspective(mVerticalFov, GetAspectRatio(), mZNear, mZFar);
    return ProjectionMatrix;
}

float Canavar::Engine::PerspectiveCamera::GetHorizontalFov() const
{
    const auto HorizontalFov = std::atan(std::tan(mVerticalFov / 2.0) / GetAspectRatio()) * 2.0f;

    return std::abs(qRadiansToDegrees(HorizontalFov));
}

float Canavar::Engine::PerspectiveCamera::GetAspectRatio() const
{
    return float(mWidth) / float(mHeight);
}
