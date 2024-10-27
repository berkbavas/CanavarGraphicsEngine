#pragma once

#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QVector3D>

namespace Canavar::Engine
{
    class Haze
    {
      private:
        friend class NodeManager;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(0.33f, 0.38f, 0.47f));
        DEFINE_MEMBER(bool, Enabled, true);
        DEFINE_MEMBER(float, Density, 1.0f);
        DEFINE_MEMBER(float, Gradient, 1.5f);
    };

    using HazePtr = std::shared_ptr<Haze>;
}
