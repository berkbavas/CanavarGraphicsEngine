#pragma once

#include "Canavar/Engine/Object/AABB.h"
#include "Canavar/Engine/Scene/TextureMaterial.h"
#include "Canavar/Engine/Scene/TriangleFace.h"
#include "Canavar/Engine/Scene/Vertex.h"

#include <memory>

#include <QMatrix4x4>
#include <QOpenGLExtraFunctions>
#include <QString>
#include <QVector>

namespace Canavar::Engine
{
    class TexturedModel;
    class Shader;

    class Mesh final : protected QOpenGLExtraFunctions
    {
      public:
        explicit Mesh(const QString &MeshName, int MeshId, const QVector<Vertex> &Vertices, const QVector<TriangleFace> &TriangleFaces, TextureMaterialPtr pTextureMaterial, const AABB &BoundingBox);
        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;
        ~Mesh();

        void Render(TexturedModel *pTexturedModel, Shader *pShader, RenderPass RenderPass, const QMatrix4x4 &NodeTransformation);

        const QString &GetMeshName() const;
        const std::string &GetMeshNameStdString() const;
        unsigned int GetNumIndices() const;
        TextureMaterialPtr GetTextureMaterial() const;
        int GetMeshId() const;
        const AABB &GetBoundingBox() const;

      private:
        bool ShouldRenderMesh(RenderPass RenderPass, float MeshOpacity) const;
        float CalculateMeshOpacity(float ModelOpacity, float PerMeshOpacityFactor) const;

        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
        GLuint mEBO{ 0 };

        QString mMeshName;
        std::string mMeshNameStdString;
        unsigned int mNumIndices{ 0 };
        TextureMaterialWeakPtr mTextureMaterial;
        int mMeshId{ -1 }; // Unique identifier for the mesh, used for selection and identification purposes
        AABB mBoundingBox; // Axis-Aligned Bounding Box for the mesh
    };

    using MeshPtr = std::shared_ptr<Mesh>;
    using MeshWeakPtr = std::weak_ptr<Mesh>;
}
