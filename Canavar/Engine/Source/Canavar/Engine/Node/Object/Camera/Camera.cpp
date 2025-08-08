#include "Camera.h"

float Canavar::Engine::Camera::CalculateSkyYOffset(float horizonDistance)
{
    mTransformCacheForSkyYOffset.setToIdentity();
    mTransformCacheForSkyYOffset.setColumn(3, QVector4D(0, GetPosition().y(), 0, 1));
    QVector4D ndc = (GetProjectionMatrix() * mTransformCacheForSkyYOffset) * QVector4D(0, 0, horizonDistance, 1);

    return ndc.y() / ndc.w();
}

QVector2D Canavar::Engine::Camera::Project3DTo2D(const QVector3D &pos3D)
{
    const auto &VP = GetViewProjectionMatrix();
    const auto &clip = VP * QVector4D(pos3D, 1.0f);

    // Perspective divide
    QVector4D ndc = clip / clip.w();

    // NDC [-1, 1] → screen coords [0, width/height]
    float x = (ndc.x() * 0.5f + 0.5f) * mWidth;
    float y = (ndc.y() * 0.5f + 0.5f) * mHeight;

    return QVector2D(x, y);
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
