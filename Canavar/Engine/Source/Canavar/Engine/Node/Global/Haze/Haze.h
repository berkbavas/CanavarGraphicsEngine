#pragma once

#include "Canavar/Engine/Node/Global/Global.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QVector3D>

namespace Canavar::Engine
{
    class Haze : public Global
    {
      public:
        CANAVAR_NODE(Haze);

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes) override;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(0.33f, 0.38f, 0.47f));
        DEFINE_MEMBER(float, Density, 1.0f);
        DEFINE_MEMBER(float, Gradient, 1.5f);
        DEFINE_MEMBER(bool, Enabled);
    };

    using HazePtr = std::shared_ptr<Haze>;
}
