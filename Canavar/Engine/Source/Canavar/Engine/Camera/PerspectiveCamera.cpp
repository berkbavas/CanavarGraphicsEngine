#include "PerspectiveCamera.h"

QMatrix4x4 Canavar::Engine::PerspectiveCamera::GetProjectionMatrix() const
{
    QMatrix4x4 ProjectionMatrix;
    ProjectionMatrix.perspective(mVerticalFov, GetAspectRatio(), mZNear, mZFar);
    return ProjectionMatrix;
}

float Canavar::Engine::PerspectiveCamera::GetHorizontalFov() const
{
    float AspectRatio = GetAspectRatio();
    float VerticalFovRad = qDegreesToRadians(mVerticalFov);
    float HorizontalFovRad = 2.0f * std::atan(std::tan(VerticalFovRad / 2.0f) * AspectRatio);
    return qRadiansToDegrees(HorizontalFovRad);
}

float Canavar::Engine::PerspectiveCamera::GetAspectRatio() const
{
    return float(mWidth) / float(mHeight);
}
