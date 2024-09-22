#pragma once

#include "Mesh.h"
#include "ModelDataNode.h"

#include <QObject>

namespace Canavar {
    namespace Engine {
        class ModelData : public QObject
        {
            Q_OBJECT
        public:
            ModelData(const QString& name);
            ~ModelData();

            void AddMesh(Mesh* mesh);
            void AddMaterial(Material* material);
            void SetRootNode(ModelDataNode* newRootNode);

            Material* GetMaterial(int index);
            Mesh* GetMeshByID(unsigned int id);

            const QString& GetName() const;
            const QVector<Mesh*>& GetMeshes() const;

            void Render(RenderModes modes, Model* model);

        private:
            QString mName;
            QVector<Mesh*> mMeshes;
            QVector<Material*> mMaterials;
            ModelDataNode* mRootNode;

            DEFINE_MEMBER(AABB, AABB);
        };
    } // namespace Engine
} // namespace Canavar
