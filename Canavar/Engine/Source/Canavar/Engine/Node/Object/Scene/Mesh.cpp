#include "Mesh.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::Mesh::~Mesh()
{
    Destroy();
}

void Canavar::Engine::Mesh::Initialize()
{
    if (mVAO != 0)
    {
        LOG_WARN("Mesh::Initialize: '{}' has been initialized already.", mMeshName);
        return;
    }

    LOG_DEBUG("Mesh::Initialize: Initializing '{}'...", mMeshName);

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_DYNAMIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    //Texture Cooords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Texture));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Tangent));
    glEnableVertexAttribArray(3);

    // Bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Bitangent));
    glEnableVertexAttribArray(4);

    // Color
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Color));
    glEnableVertexAttribArray(5);

    // Mask
    glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void *) offsetof(Vertex, Mask));
    glEnableVertexAttribArray(6);

    glBindVertexArray(0);

    mNumberOfVertices = mVertices.size();

    LOG_DEBUG("Mesh::Initialize: Initialization of '{}' is done.", mMeshName);
}

void Canavar::Engine::Mesh::Destroy()
{
    if (mVAO)
    {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
    }

    if (mVBO)
    {
        glDeleteBuffers(1, &mVBO);
        mVBO = 0;
    }

    if (mEBO)
    {
        glDeleteBuffers(1, &mEBO);
        mEBO = 0;
    }
}

void Canavar::Engine::Mesh::Render(Model *pModel, Shader *pShader, const QMatrix4x4 &Node4x4, RenderPass RenderPass)
{
    if (mVAO == 0)
    {
        LOG_FATAL("Mesh::Render: '{}' is not initialized yet. Returning...", mMeshName);
        return;
    }

    if (!ShouldRender(pModel, RenderPass))
    {
        return;
    }

    const auto &Model4x4 = pModel->GetWorldTransformation();
    const auto &Mesh4x4 = pModel->GetMeshTransformation(GetUniqueMeshName());
    const auto &MeshOpacity = pModel->GetMeshOpacity(GetUniqueMeshName(), mOpacity);

    const auto M = Model4x4 * (Mesh4x4 * Node4x4);

    pShader->SetUniformValue("uModelMatrix", M);

    if (pShader->GetShaderType() == ShaderType::ShadowMapping)
    {
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    else if (pShader->GetShaderType() == ShaderType::CrossSection)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else
    {
        pShader->SetUniformValue("uMeshOpacity", MeshOpacity);

        pShader->SetUniformValue("uNormalMatrix", M.normalMatrix());
        pShader->SetUniformValue("uNodeId", pModel->GetNodeId());
        pShader->SetUniformValue("uMeshId", mMeshId);

        pShader->SetUniformValue("uModel.Metallic", pModel->GetMetallic());
        pShader->SetUniformValue("uModel.Roughness", pModel->GetRoughness());
        pShader->SetUniformValue("uModel.AmbientOcclusion", pModel->GetAmbientOcclusion());

        pShader->SetUniformValue("uHasTextureBaseColor", mMaterial->HasTextureBaseColor());
        pShader->SetUniformValue("uHasTextureMetallic", mMaterial->HasTextureMetallic());
        pShader->SetUniformValue("uHasTextureRoughness", mMaterial->HasTextureRoughness());
        pShader->SetUniformValue("uHasTextureAmbientOcclusion", mMaterial->HasTextureAmbientOcclusion());
        pShader->SetUniformValue("uHasTextureNormal", mMaterial->HasTextureNormal());

        pShader->SetSampler("uTextureBaseColor", 0, mMaterial->GetTexture(TextureType::BaseColor));
        pShader->SetSampler("uTextureMetallic", 1, mMaterial->GetTexture(TextureType::Metallic));
        pShader->SetSampler("uTextureRoughness", 2, mMaterial->GetTexture(TextureType::Roughness));
        pShader->SetSampler("uTextureAmbientOcclusion", 3, mMaterial->GetTexture(TextureType::AmbientOcclusion));
        pShader->SetSampler("uTextureNormal", 4, mMaterial->GetTexture(TextureType::Normal));

        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Canavar::Engine::Mesh::UnpaintVertex(unsigned int Index)
{
    if (Index >= mNumberOfVertices)
    {
        LOG_FATAL("Mesh::UnpaintVertex: Index '{}' is out of bounds.", Index);
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    Vertex *pVertices = static_cast<Vertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
    pVertices[Index].Mask = VERTEX_MASK_NO_MASK;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Canavar::Engine::Mesh::PaintVertex(unsigned int Index, const QVector3D &Color)
{
    if (Index >= mNumberOfVertices)
    {
        LOG_FATAL("Mesh::PaintVertex: Index '{}' is out of bounds.", Index);
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    Vertex *pVertices = static_cast<Vertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
    pVertices[Index].Mask = VERTEX_MASK_PAINTED;
    pVertices[Index].Color = Color;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Canavar::Engine::Mesh::AddVertex(const Vertex &Vertex)
{
    mVertices.emplace_back(Vertex);
}

void Canavar::Engine::Mesh::AddIndex(unsigned int Index)
{
    mIndices.push_back(Index);
}

std::tuple<unsigned int, unsigned int, unsigned int> Canavar::Engine::Mesh::GetTriangleVertices(unsigned int PrimitiveIndex) const
{
    if (PrimitiveIndex * 3 + 2 < mIndices.size())
    {
        return std::make_tuple(mIndices[PrimitiveIndex * 3], mIndices[PrimitiveIndex * 3 + 1], mIndices[PrimitiveIndex * 3 + 2]);
    }
    else
    {
        LOG_FATAL("Mesh::GetTriangleVertices: PrimitiveIndex '{}' is out of bounds. Indices size: '{}'", PrimitiveIndex, mIndices.size());
        return std::tuple<unsigned int, unsigned int, unsigned int>();
    }
}

const std::string &Canavar::Engine::Mesh::GetUniqueMeshName()
{
    mUniqueMeshName = mMeshName + "_" + std::to_string(mMeshId);
    return mUniqueMeshName;
}

bool Canavar::Engine::Mesh::HasTransparency(const Model *pModel) const
{
    return pModel->GetMeshOpacity(mUniqueMeshName, mOpacity) < 1.0f;
}

bool Canavar::Engine::Mesh::ShouldRender(const Model *pModel, RenderPass RenderPass) const
{
    if (pModel->GetMeshVisibility(mUniqueMeshName) == false)
    {
        return false;
    }

    switch (RenderPass)
    {
    case RenderPass::Opaque:
        if (HasTransparency(pModel))
        {
            return false;
        }
        break;
    case RenderPass::Transparent:
        if (!HasTransparency(pModel))
        {
            return false;
        }
        break;

    default:
        break;
    }

    return true;
}
