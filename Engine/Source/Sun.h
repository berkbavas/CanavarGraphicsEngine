#pragma once

#include "Light.h"

namespace Canavar {
    namespace Engine {
        class Sun : public Light
        {
        private:
            Sun();

        protected:
            friend class NodeManager;
            virtual void ToJson(QJsonObject& object) override;
            virtual void FromJson(const QJsonObject& object) override;

        public:
            static Sun* Instance();

        protected:
            DEFINE_MEMBER(bool, Enabled);
            DEFINE_MEMBER(QVector3D, Direction);
        };
    } // namespace Engine
} // namespace Canavar
