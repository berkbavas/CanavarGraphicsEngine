#pragma once

#include "Canavar/Engine/Manager/Manager.h"

#include <QList>
#include <QVector>

namespace Canavar::Engine
{
    class DirectionalLight;
    class PointLight;
    class Light;

    class LightManager : public Manager
    {
      public:
        LightManager() = default;

        QVector<PointLight *> GetPointLightsAround(QVector3D TargetPosition, float Radius);
        const QList<PointLight *> &GetPointLights() const;
        const QList<DirectionalLight *> &GetDirectionalLights() const;
        void AddLight(Light *pLight);
        void RemoveLight(Light *pLight);

      private:
        QList<DirectionalLight *> mDirectionalLights;
        QList<PointLight *> mPointLights;
    };

    using LightManagerPtr = std::unique_ptr<LightManager>;
}