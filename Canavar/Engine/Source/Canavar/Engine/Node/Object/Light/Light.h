#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

namespace Canavar::Engine
{
    class Light : public Object
    {
      protected:
        Light() = default;

        void ToJson(QJsonObject &Object) override;
        void FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes) override;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Ambient, 1.0f);
        DEFINE_MEMBER(float, Diffuse, 1.0f);
        DEFINE_MEMBER(float, Specular, 1.0f);
        DEFINE_MEMBER(bool, Enabled, true);
    };

    using LightPtr = std::shared_ptr<Light>;
}