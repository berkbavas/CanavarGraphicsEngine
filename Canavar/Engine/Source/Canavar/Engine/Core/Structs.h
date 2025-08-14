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

    struct TransparentMeshListElement
    {
        Model *pModel;
        Mesh *pMesh;
        QMatrix4x4 NodeMatrix;
        QMatrix4x4 CombinedTransformation;
    };

    struct Mouse
    {
        float x;
        float y;
        float z;
        float w;
        float dx;
        float dy;
        float dz;
        float dw;

        Qt::MouseButton button;

        Mouse() { Reset(); }

        void Reset()
        {
            x = 0;
            y = 0;
            z = 0;
            w = 0;
            dx = 0;
            dy = 0;
            dz = 0;
            dw = 0;
            button = Qt::NoButton;
        }
    };
}