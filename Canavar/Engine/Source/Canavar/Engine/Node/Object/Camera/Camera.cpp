#include "Camera.h"

QVector2D Canavar::Engine::Camera::ProjectWorldToScreenSpace(const QVector3D &WorldPosition) const
{
    const auto &VP = GetViewProjectionMatrix();
    const auto &Clip = VP * QVector4D(WorldPosition, 1.0f);

    // Perspective divide
    QVector4D NDC = Clip / Clip.w();

    // NDC [-1, 1] → screen coords [0, width/height]
    float x = (NDC.x() * 0.5f + 0.5f) * mWidth;
    float y = (NDC.y() * 0.5f + 0.5f) * mHeight;

    return QVector2D(x, y);
}

QVector3D Canavar::Engine::Camera::UnprojectScreenToWorldSpace(const QVector2D &ScreenPos, float Depth) const
{
    // Convert screen position to Normalized Device Coordinates (NDC)
    float x = (2.0f * ScreenPos.x()) / mWidth - 1.0f;
    float y = 1.0f - (2.0f * ScreenPos.y()) / mHeight; // Invert Y for NDC
    QVector4D RayNDC(x, y, -1.0f, 1.0f);               // z = -1 for near plane

    // Convert NDC to eye coordinates
    QMatrix4x4 InverseProjection = GetProjectionMatrix().inverted();
    QVector4D RayEye = InverseProjection * RayNDC;
    RayEye.setZ(-1.0f); // Forward direction in eye space
    RayEye.setW(0.0f);  // Direction vector

    // Convert eye coordinates to world coordinates
    QMatrix4x4 InverseView = GetViewMatrix().inverted();
    QVector4D RayWorld = InverseView * RayEye;

    return QVector3D(RayWorld.x(), RayWorld.y(), RayWorld.z());
}

QVector3D Canavar::Engine::Camera::ComputeRayFromScreenPosition(const QPointF &ScreenPos) const
{
    // Convert screen position to Normalized Device Coordinates (NDC)
    float x = (2.0f * ScreenPos.x()) / mWidth - 1.0f;
    float y = 1.0f - (2.0f * ScreenPos.y()) / mHeight; // Invert Y for NDC
    QVector4D RayNDC(x, y, -1.0f, 1.0f);               // z = -1 for near plane

    // Convert NDC to eye coordinates
    QMatrix4x4 InverseProjection = GetProjectionMatrix().inverted();
    QVector4D RayEye = InverseProjection * RayNDC;
    RayEye.setZ(-1.0f); // Forward direction in eye space
    RayEye.setW(0.0f);  // Direction vector

    // Convert eye coordinates to world coordinates
    QMatrix4x4 InverseView = GetViewMatrix().inverted();
    QVector4D RayWorld = InverseView * RayEye;

    QVector3D RayDirection(RayWorld.x(), RayWorld.y(), RayWorld.z());
    RayDirection.normalize();

    return RayDirection;
}

void Canavar::Engine::Camera::Update(float ifps) {}

void Canavar::Engine::Camera::Reset() {}

void Canavar::Engine::Camera::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

int Canavar::Engine::Camera::GetWidth() const
{
    return mWidth;
}

int Canavar::Engine::Camera::GetHeight() const
{
    return mHeight;
}
