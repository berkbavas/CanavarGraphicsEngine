#pragma once

#include "Canavar/Engine/Node/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Util/Mouse.h"

#include <QMap>
#include <QMouseEvent>

namespace Canavar::Engine
{
    class FreeCamera : public PerspectiveCamera
    {
      public:
        FreeCamera();

        void Update(float ifps);
        void Reset();

        void KeyPressed(QKeyEvent *) override;
        void KeyReleased(QKeyEvent *) override;
        void MousePressed(QMouseEvent *) override;
        void MouseReleased(QMouseEvent *) override;
        void MouseMoved(QMouseEvent *) override;

        void SetParent(NodeWeakPtr pParentNode) override;
        void AddChild(NodePtr pNode) override;
        void RemoveChild(NodePtr pNode) override;

      private:
        DEFINE_MEMBER(float, AngularSpeed, 25.0f);
        DEFINE_MEMBER(float, LinearSpeed, 5.0f);
        DEFINE_MEMBER(float, LinearSpeedMultiplier, 1.0f);
        DEFINE_MEMBER(float, AngularSpeedMultiplier, 1.0f);
        DEFINE_MEMBER(Qt::MouseButton, ActionReceiveButton, Qt::MiddleButton)

        QMap<Qt::Key, bool> mPressedKeys;

        Mouse mMouse;

        bool mUpdateRotation{ false };
        bool mUpdatePosition{ false };

        static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
    };

    using FreeCameraPtr = std::shared_ptr<FreeCamera>;

}
