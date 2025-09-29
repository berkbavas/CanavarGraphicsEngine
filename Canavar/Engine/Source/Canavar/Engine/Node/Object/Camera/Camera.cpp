#include "Camera.h"

QVector2D Canavar::Engine::Camera::Project3DTo2D(const QVector3D &Pos3D)
{
    const auto &VP = GetViewProjectionMatrix();
    const auto &Clip = VP * QVector4D(Pos3D, 1.0f);

    // Perspective divide
    QVector4D NDC = Clip / Clip.w();

    // NDC [-1, 1] → screen coords [0, width/height]
    float x = (NDC.x() * 0.5f + 0.5f) * mWidth;
    float y = (NDC.y() * 0.5f + 0.5f) * mHeight;

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
