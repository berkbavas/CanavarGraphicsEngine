#pragma once

#include <QVector2D>
#include <QVector3D>

namespace Canavar::Engine
{
#pragma pack(push, 1)
    struct Vertex
    {
        QVector3D Position;
        QVector3D Normal;
        QVector2D UV;
        QVector3D Tangent;
        QVector3D Bitangent;
    };
#pragma pack(pop)
}