#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Light/PointLight.h"

#include <set>

#include <QVector>

namespace Canavar::Engine
{
    class LightManager : public Manager
    {
      public:
        explicit LightManager(QObject *parent = nullptr);

        void Initialize() override;

        void AddLight(LightPtr pLight);
        void RemoveLight(LightPtr pLight);

        std::vector<PointLightPtr> GetPointLightsAround(QVector3D targetPosition, float radius);
        const QVector<DirectionalLightPtr> &GetDirectionalLights() const;

      private:
        std::set<PointLightPtr> mPointLights;
        QVector<DirectionalLightPtr> mDirectionalLights;

        DEFINE_MEMBER_CONST(DirectionalLightPtr, Sun);
    };

}