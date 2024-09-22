#pragma once

#include "Common.h"
#include "Node.h"

#include <QKeyEvent>

namespace Canavar {
    namespace Engine {
        class Camera : public Node
        {
            Q_OBJECT
        protected:
            Camera();
            virtual void ToJson(QJsonObject& object) override;
            virtual void FromJson(const QJsonObject& object) override;

        public:
            virtual QMatrix4x4 GetViewProjectionMatrix();
            virtual QMatrix4x4 GetViewMatrix();
            virtual QMatrix4x4 GetRotationMatrix();
            virtual QVector3D GetViewDirection();
            virtual QMatrix4x4 GetProjectionMatrix() = 0;
            virtual float CalculateSkyYOffset(float horizonDistance);

            virtual void MouseDoubleClicked(QMouseEvent*) = 0;
            virtual void MousePressed(QMouseEvent*) = 0;
            virtual void MouseReleased(QMouseEvent*) = 0;
            virtual void MouseMoved(QMouseEvent*) = 0;
            virtual void WheelMoved(QWheelEvent*) = 0;
            virtual void KeyPressed(QKeyEvent*) = 0;
            virtual void KeyReleased(QKeyEvent*) = 0;
            virtual void Update(float) = 0;
            virtual void Reset() = 0;

            virtual void Resize(int width, int height);

            bool GetActive() const;
            void SetActive(bool newActive);

        signals:
            void ActiveChanged();

        protected:
            bool mActive;

            DEFINE_MEMBER(int, Width);
            DEFINE_MEMBER(int, Height);
            DEFINE_MEMBER(float, ZNear);
            DEFINE_MEMBER(float, ZFar);
        };
    } // namespace Engine
} // namespace Canavar
