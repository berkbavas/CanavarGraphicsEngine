#pragma once

#include "AABB.h"
#include "Common.h"
#include "Material.h"

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>

namespace Canavar {
    namespace Engine {
        class CameraManager;
        class ShaderManager;
        class LightManager;

        class Camera;
        class Sun;

        class Mesh : public QObject, protected QOpenGLExtraFunctions
        {
            Q_OBJECT
        public:
            struct Vertex {
                QVector3D position;
                QVector3D normal;
                QVector2D texture;
                QVector3D tangent;
                QVector3D bitangent;
                int boneIDs[4];
                float weights[4];
            };

            Mesh();
            virtual ~Mesh();

            void AddVertex(const Vertex& vertex);
            void AddIndex(unsigned int index);
            void SetMaterial(Material* material);
            void Create();
            void Render(RenderModes modes, Model* model);

            Vertex GetVertex(int index) const;
            int GetNumberOfVertices();

            QOpenGLVertexArrayObject* GetVerticesVAO() const;

        private:
            QOpenGLVertexArrayObject* mVAO;
            unsigned int mEBO;
            unsigned int mVBO;

            QVector<Vertex> mVertices;
            QVector<unsigned int> mIndices;
            Material* mMaterial;

            DEFINE_MEMBER(AABB, AABB);
            DEFINE_MEMBER(QString, Name);
            DEFINE_MEMBER(unsigned int, ID);

            // For rendering
            ShaderManager* mShaderManager;
            CameraManager* mCameraManager;

            // For vertex rendering
            QOpenGLVertexArrayObject* mVerticesVAO;
            unsigned int mVerticesVBO;
        };
    } // namespace Engine
} // namespace Canavar
