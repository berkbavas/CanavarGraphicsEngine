#pragma once

#include <QVector3D>

namespace Canavar::Engine
{
    struct Vertex
    {
        QVector4D position;
        QVector4D normal;
        QVector4D texture;
        QVector4D tangent;
        QVector4D bitangent;
        QVector4D color;
    };
}
