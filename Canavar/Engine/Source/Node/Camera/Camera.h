#pragma once

#include "Node/Node.h"

#include <QMouseEvent>

namespace Canavar::Engine
{
    class Camera : public Node
    {
      protected:
        Camera() = default;

      public:
        virtual const QMatrix4x4 &GetProjectionMatrix() = 0;
        virtual const QMatrix4x4 &GetViewProjectionMatrix() = 0;
        virtual const QMatrix4x4 &GetRotationMatrix() = 0;
        virtual const QMatrix4x4 &GetViewMatrix() = 0;
        virtual const QVector3D &GetViewDirection() = 0;
        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;

        virtual void Update(float ifps);
        virtual void Reset();
        virtual void Resize(int w, int h);

        virtual void KeyPressed(QKeyEvent *);
        virtual void KeyReleased(QKeyEvent *);
        virtual void MousePressed(QMouseEvent *);
        virtual void MouseReleased(QMouseEvent *);
        virtual void MouseMoved(QMouseEvent *);
        virtual void WheelMoved(QWheelEvent *);
    };

    using CameraPtr = std::shared_ptr<Camera>;
}