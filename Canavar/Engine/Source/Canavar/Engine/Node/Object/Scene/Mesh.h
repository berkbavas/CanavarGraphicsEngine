#pragma once

#include "Canavar/Engine/Core/AABB.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"
#include "Canavar/Engine/Node/Object/Scene/Material.h"
#include "Canavar/Engine/Node/Object/Scene/Vertex.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>
#include <vector>

#include <QOpenGLFunctions_4_5_Core>
#include <QVector3D>

namespace Canavar::Engine
{
    class Mesh : protected QOpenGLFunctions_4_5_Core
    {
        DISABLE_COPY(Mesh);

      public:
        Mesh() = default;
        virtual ~Mesh();

        void Initialize();
        void Destroy();
        void Render(Model *pModel, Shader *pShader, const QMatrix4x4 &Node4x4, RenderPass RenderPass);

        void UnpaintVertex(unsigned int Index);
        void PaintVertex(unsigned int Index, const QVector3D &Color);

        void AddVertex(const Vertex &Vertex);
        void AddIndex(unsigned int Index);

        std::tuple<unsigned int, unsigned int, unsigned int> GetTriangleVertices(unsigned int PrimitiveIndex) const;

        const std::string &GetUniqueMeshName();

        bool HasTransparency(const Model *pModel) const;
      private:
        bool ShouldRender(const Model *pModel, RenderPass RenderPass) const;

        DEFINE_MEMBER_CONST(GLuint, VAO, 0);
        DEFINE_MEMBER_CONST(GLuint, VBO, 0);
        DEFINE_MEMBER_CONST(GLuint, EBO, 0);

        DEFINE_MEMBER_CONST(std::vector<Vertex>, Vertices);
        DEFINE_MEMBER_CONST(std::vector<unsigned int>, Indices);

        DEFINE_MEMBER(MaterialPtr, Material, nullptr);
        DEFINE_MEMBER(std::string, MeshName);
        DEFINE_MEMBER(int, MeshId);
        DEFINE_MEMBER(AABB, AABB);
        DEFINE_MEMBER(float, Opacity, 1.0f);

        DEFINE_MEMBER_CONST(unsigned int, NumberOfVertices, 0);

        std::string mUniqueMeshName;
    };

    using MeshPtr = std::shared_ptr<Mesh>;
}
