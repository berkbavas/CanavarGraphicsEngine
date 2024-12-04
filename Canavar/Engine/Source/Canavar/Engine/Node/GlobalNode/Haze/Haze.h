#pragma once

#include "Canavar/Engine/Node/GlobalNode/GlobalNode.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QVector3D>

namespace Canavar::Engine
{
    class Haze : public GlobalNode
    {
        REGISTER_NODE_TYPE(Haze);

      public:
        Haze();

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes) override;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(0.33f, 0.38f, 0.47f));
        DEFINE_MEMBER(float, Density, 1.0f);
        DEFINE_MEMBER(float, Gradient, 1.5f);
    };

    using HazePtr = std::shared_ptr<Haze>;
}
