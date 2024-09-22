#pragma once

#include "Manager.h"
#include "NodeManager.h"
#include "PointLight.h"

#include <QObject>

namespace Canavar {
    namespace Engine {
        class LightManager : public Manager
        {
        private:
            friend class Canavar::Engine::NodeManager;
            LightManager();

        public:
            static LightManager* Instance();
            bool Init() override;

            void AddLight(Light* light);
            void RemoveLight(Light* light);
            const QList<PointLight*>& GetPointLights() const;

        private:
            QList<PointLight*> mPointLights;
        };
    } // namespace Engine
} // namespace Canavar
