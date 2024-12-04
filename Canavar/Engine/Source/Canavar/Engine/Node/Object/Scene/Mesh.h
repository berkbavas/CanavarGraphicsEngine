#pragma once

#include "Canavar/Engine/Manager/RenderingManager/Shader.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"
#include "Canavar/Engine/Node/Object/Scene/AABB.h"
#include "Canavar/Engine/Node/Object/Scene/Material.h"
#include "Canavar/Engine/Node/Object/Scene/Vertex.h"
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
        virtual ~Mesh();

        void Initialize();
        void Destroy();
        void Render(Model *pModel, Shader *pShader);

        void AddVertex(const Vertex &vertex);
        void AddIndex(unsigned int index);

      private:
        DEFINE_MEMBER_CONST(GLuint, VAO, 0);
        DEFINE_MEMBER_CONST(GLuint, VBO, 0);
        DEFINE_MEMBER_CONST(GLuint, EBO, 0);

        DEFINE_MEMBER_CONST(std::vector<Vertex>, Vertices);
        DEFINE_MEMBER_CONST(std::vector<unsigned int>, Indices);

        DEFINE_MEMBER(MaterialPtr, Material, nullptr);
        DEFINE_MEMBER(QString, MeshName);
        DEFINE_MEMBER(uint32_t, MeshId);
        DEFINE_MEMBER(AABB, AABB);
    };

    using MeshPtr = std::shared_ptr<Mesh>;
}
