#pragma once

#include "Canavar/Engine/Core/Structs.h"
#include "Canavar/Engine/Node/Object/Camera/FreeCameraSlerpAnimator.h"
#include "Canavar/Engine/Node/Object/Camera/PerspectiveCamera.h"

#include <QMap>
#include <QMouseEvent>
#include <QObject>

namespace Canavar::Engine
{
    class FreeCamera : public PerspectiveCamera, public QObject
    {
      public:
        FreeCamera();

        const char *GetNodeTypeName() const override { return "FreeCamera"; }
        void Accept(NodeVisitor& visitor) override;

        void Update(float ifps) override;
        void Reset() override;

        bool KeyPressed(QKeyEvent *) override;
        bool KeyReleased(QKeyEvent *) override;
        bool MousePressed(QMouseEvent *) override;
        bool MouseReleased(QMouseEvent *) override;
        bool MouseMoved(QMouseEvent *) override;

        void GoToObject(Object* pNode);

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes) override;

      private:
        DEFINE_MEMBER(float, AngularSpeed, 25.0f);
        DEFINE_MEMBER(float, AngularSpeedSmoothness, 0.5f);
        DEFINE_MEMBER(float, LinearSpeed, 5.0f);
        DEFINE_MEMBER(Qt::MouseButton, ActionReceiveButton, Qt::MiddleButton)

        QMap<Qt::Key, bool> mPressedKeys;

        Mouse mMouse;

        bool mUpdateRotation{ false };
        bool mUpdatePosition{ false };

        FreeCameraSlerpAnimatorPtr mAnimator{ nullptr };

        static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;

        static constexpr QVector3D POSITIVE_Z = QVector3D(0, 0, 1);
        static constexpr QVector3D NEGATIVE_Z = QVector3D(0, 0, -1);
    };

    using FreeCameraPtr = std::shared_ptr<FreeCamera>;

}
