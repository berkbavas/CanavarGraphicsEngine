#pragma once

#include "Camera.h"

namespace Canavar {
    namespace Engine {
        class PerspectiveCamera : public Camera
        {
        protected:
            PerspectiveCamera();
            virtual void ToJson(QJsonObject& object) override;
            virtual void FromJson(const QJsonObject& object) override;

        public:
            virtual QMatrix4x4 GetProjectionMatrix() override;

            const float& GetVerticalFov() const;
            float& GetVerticalFov_NonConst();
            void SetVerticalFov(float newVerticalFov);

            const float& GetHorizontalFov() const;
            float& GetHorizontalFov_NonConst();
            void SetHorizontalFov(float newHorizontalFov);

        protected:
            float mVerticalFov;
            float mHorizontalFov;
        };
    } // namespace Engine
} // namespace Canavar
