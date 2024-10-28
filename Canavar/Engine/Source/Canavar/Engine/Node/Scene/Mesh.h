#pragma once

#include "Canavar/Engine/Manager/RenderingManager/Shader.h"
#include "Canavar/Engine/Node/Model/Model.h"
#include "Canavar/Engine/Node/Scene/AABB.h"
#include "Canavar/Engine/Node/Scene/Material.h"
#include "Canavar/Engine/Node/Scene/Vertex.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>
#include <vector>

#include <QOpenGLExtraFunctions>
#include <QVector3D>

namespace Canavar::Engine
{
    class Mesh : protected QOpenGLExtraFunctions
    {
        DISABLE_COPY(Mesh);

      public:
        Mesh() = default;

        void Initialize();
        void Destroy();
        void Render(Model *pModel, Shader *pShader);

        void AddVertex(const Vertex &vertex);
        void AddIndex(unsigned int index);

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
        GLuint mEBO{ 0 };

        std::vector<Vertex> mVertices;
        std::vector<unsigned int> mIndices;

        DEFINE_MEMBER(MaterialPtr, Material, nullptr);
        DEFINE_MEMBER(QString, MeshName);
        DEFINE_MEMBER(uint32_t, MeshId);
        DEFINE_MEMBER(AABB, AABB);
    };

    using MeshPtr = std::shared_ptr<Mesh>;
}
