#pragma once

#include "Common.h"
#include "Node.h"

namespace Canavar {
    namespace Engine {
        class Haze : public Node
        {
        private:
            Haze();

        protected:
            friend class NodeManager;
            virtual void ToJson(QJsonObject& object) override;
            virtual void FromJson(const QJsonObject& object) override;

        public:
            static Haze* Instance();

            DEFINE_MEMBER(QVector3D, Color);
            DEFINE_MEMBER(bool, Enabled);
            DEFINE_MEMBER(float, Density);
            DEFINE_MEMBER(float, Gradient);
        };
    } // namespace Engine
} // namespace Canavar
