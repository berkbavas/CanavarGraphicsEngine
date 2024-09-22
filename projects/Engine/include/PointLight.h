#pragma once

#include "Light.h"

namespace Canavar {
    namespace Engine {
        class NodeManager;

        class PointLight : public Light
        {
        protected:
            friend class NodeManager;

            PointLight();

            virtual void ToJson(QJsonObject& object) override;
            virtual void FromJson(const QJsonObject& object) override;

            DEFINE_MEMBER(float, Constant);
            DEFINE_MEMBER(float, Linear);
            DEFINE_MEMBER(float, Quadratic);
        };
    } // namespace Engine
} // namespace Canavar
