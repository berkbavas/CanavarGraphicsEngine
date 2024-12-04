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

void Canavar::Engine::Camera::KeyPressed(QKeyEvent *) {}

void Canavar::Engine::Camera::KeyReleased(QKeyEvent *) {}

void Canavar::Engine::Camera::MousePressed(QMouseEvent *) {}

void Canavar::Engine::Camera::MouseReleased(QMouseEvent *) {}

void Canavar::Engine::Camera::MouseMoved(QMouseEvent *) {}

void Canavar::Engine::Camera::WheelMoved(QWheelEvent *) {}

void Canavar::Engine::Camera::ToJson(QJsonObject &object)
{
    Object::ToJson(object);
}

void Canavar::Engine::Camera::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    Object::FromJson(object, nodes);
}
