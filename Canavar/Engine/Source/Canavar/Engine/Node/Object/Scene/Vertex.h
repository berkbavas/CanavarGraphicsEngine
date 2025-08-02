#pragma once

#include <QVector3D>

namespace Canavar::Engine
{
    struct Vertex
    {
        QVector3D position;
        QVector3D normal;
        QVector2D texture;
        QVector3D tangent;
        QVector3D bitangent;
    };
}
