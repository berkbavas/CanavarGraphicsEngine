#pragma once

#include "Node.h"

#include <QOpenGLFunctions>

namespace Canavar {
    namespace Engine {
        class ModelData;

        class ModelDataNode : public Node
        {
        public:
            ModelDataNode(ModelData* data);

            void AddMeshIndex(int index);

            void Render(RenderModes modes, Model* model);

        private:
            ModelData* mModelData;
            QVector<int> mMeshIndices;
        };
    } // namespace Engine
} // namespace Canavar
