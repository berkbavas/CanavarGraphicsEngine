#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Light/PointLight.h"

#include <QVector>

namespace Canavar::Engine
{
    class LightManager : public Manager
    {
      public:
        explicit LightManager(QObject *parent = nullptr);

        void Initialize() override;

        QVector<PointLightPtr> GetPointLightsAround(QVector3D targetPosition, float radius);

        const QVector<PointLightPtr> &GetPointLights() const;
        const QVector<DirectionalLightPtr> &GetDirectionalLights() const;

      private:
        void AddLight(LightPtr pLight);
        void RemoveLight(LightPtr pLight);

        QVector<PointLightPtr> mPointLights;
        QVector<DirectionalLightPtr> mDirectionalLights;

        friend class NodeManager;
    };

}