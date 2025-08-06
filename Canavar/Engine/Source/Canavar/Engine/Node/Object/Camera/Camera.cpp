#include "Camera.h"

float Canavar::Engine::Camera::CalculateSkyYOffset(float horizonDistance)
{
    mTransformCacheForSkyYOffset.setToIdentity();
    mTransformCacheForSkyYOffset.setColumn(3, QVector4D(0, GetPosition().y(), 0, 1));
    QVector4D ndc = (GetProjectionMatrix() * mTransformCacheForSkyYOffset) * QVector4D(0, 0, horizonDistance, 1);

    return ndc.y() / ndc.w();
}

void Canavar::Engine::Camera::Update(float ifps) {}

void Canavar::Engine::Camera::Reset() {}

void Canavar::Engine::Camera::Resize(int w, int h) {}