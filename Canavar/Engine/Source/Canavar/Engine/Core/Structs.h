#pragma once

#include "Canavar/Engine/Node/Object/Scene/Mesh.h"

#include <QObject>

namespace Canavar::Engine
{
    struct NodeInfo
    {
        float NodeId;
        float MeshId;
        float PrimitiveId;
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
        float dx;
        float dy;

        Qt::MouseButton button;

        Mouse() { Reset(); }

        void Reset()
        {
            x = 0;
            y = 0;
            dx = 0;
            dy = 0;
            button = Qt::NoButton;
        }
    };
}