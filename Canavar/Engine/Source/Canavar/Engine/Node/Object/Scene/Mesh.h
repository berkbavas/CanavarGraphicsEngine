#pragma once

#include "Canavar/Engine/Core/AABB.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"
#include "Canavar/Engine/Node/Object/Scene/Material.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>
#include <vector>

#include <QOpenGLExtraFunctions>
#include <QVector3D>

namespace Canavar::Engine
{
    struct Vertex
    {
        QVector3D position;
        QVector3D normal;
        QVector2D texture;
        QVector3D tangent;
        QVector3D bitangent;
    };

    class Mesh : protected QOpenGLExtraFunctions
    {
        DISABLE_COPY(Mesh);

      public:
        Mesh() = default;
        virtual ~Mesh();

        void Initialize();
        void Destroy();
        void Render(Model *pModel, Shader *pShader, const QMatrix4x4 &Node4x4, RenderPass renderPass);

        void AddVertex(const Vertex &vertex);
        void AddIndex(unsigned int index);

      private:
        bool ShouldRender(RenderPass renderPass) const;

        DEFINE_MEMBER_CONST(GLuint, VAO, 0);
        DEFINE_MEMBER_CONST(GLuint, VBO, 0);
        DEFINE_MEMBER_CONST(GLuint, EBO, 0);

        DEFINE_MEMBER_CONST(std::vector<Vertex>, Vertices);
        DEFINE_MEMBER_CONST(std::vector<unsigned int>, Indices);

        DEFINE_MEMBER(MaterialPtr, Material, nullptr);
        DEFINE_MEMBER(QString, MeshName);
        DEFINE_MEMBER(uint32_t, MeshId);
        DEFINE_MEMBER(AABB, AABB);
        DEFINE_MEMBER(bool, HasTransparency, false);
    };

    using MeshPtr = std::shared_ptr<Mesh>;
}
