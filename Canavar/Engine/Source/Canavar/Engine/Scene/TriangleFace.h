#pragma once

namespace Canavar::Engine
{
#pragma pack(push, 1)
    struct TriangleFace
    {
        unsigned int Idx0;
        unsigned int Idx1;
        unsigned int Idx2;
    };
#pragma pack(pop)
}