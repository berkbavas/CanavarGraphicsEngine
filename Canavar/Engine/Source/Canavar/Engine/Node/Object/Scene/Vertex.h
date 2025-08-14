#pragma once

namespace Canavar::Engine
{
    inline constexpr unsigned int VERTEX_MASK_NO_MASK = 0x0;
    inline constexpr unsigned int VERTEX_MASK_PAINTED = 0x1;

    // Vertex structure definition
    // 1 byte alignment
#pragma pack(push, 1)
    struct Vertex
    {
        QVector3D Position;
        QVector3D Normal;
        QVector2D Texture;
        QVector3D Tangent;
        QVector3D Bitangent;
        QVector3D Color;
        unsigned int Mask{ VERTEX_MASK_NO_MASK };
    };
#pragma pack(pop)

}