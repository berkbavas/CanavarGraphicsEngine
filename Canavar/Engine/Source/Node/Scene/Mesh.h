#pragma once

#include "Node/Model/Model.h"
#include "Node/Scene/Material.h"
#include "Node/Scene/Vertex.h"
#include "Util/Macros.h"
#include "Util/Shader.h"

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
    };

    using MeshPtr = std::shared_ptr<Mesh>;
}
