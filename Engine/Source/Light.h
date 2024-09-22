#pragma once

#include "Node.h"

namespace Canavar {
    namespace Engine {
        class Light : public Node
        {
        protected:
            Light();

            virtual void ToJson(QJsonObject& object) override;
            virtual void FromJson(const QJsonObject& object) override;

            DEFINE_MEMBER(QVector4D, Color);
            DEFINE_MEMBER(float, Ambient);
            DEFINE_MEMBER(float, Diffuse);
            DEFINE_MEMBER(float, Specular);
        };
    } // namespace Engine
} // namespace Canavar
