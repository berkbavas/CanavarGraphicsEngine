#pragma once

#include "Canavar/Engine/Node/Object/Scene/Mesh.h"

#include <QObject>

namespace Canavar::Engine
{
    struct NodeInfo
    {
        float NodeId;
        float MeshId;
        float PrimitiveIndex;
        float Success;
    };

    struct Mouse
    {
        float X;
        float Y;
        float Z;
        float W;
        float DX;
        float DY;
        float DZ;
        float DW;

        Qt::MouseButton Button;

        Mouse() { Reset(); }

        void Reset()
        {
            X = 0;
            Y = 0;
            Z = 0;
            W = 0;
            DX = 0;
            DY = 0;
            DZ = 0;
            DW = 0;
            Button = Qt::NoButton;
        }
    };
}