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
        LOG_WARN("Mesh::Initialize: '{}' has been initialized already.", mMeshName.toStdString());
        return;
    }

    LOG_DEBUG("Mesh::Initialize: Initializing '{}'...", mMeshName.toStdString());

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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    //Texture Cooords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);

    LOG_DEBUG("Mesh::Initialize: Initialization of '{}' is done.", mMeshName.toStdString());
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

void Canavar::Engine::Mesh::Render(Model *pModel, Shader *pShader, const QMatrix4x4 &Node4x4)
{
    if (mVAO == 0)
    {
        LOG_FATAL("Mesh::Render: '{}' is not initialized yet. Returning...", mMeshName.toStdString());
        return;
    }

    const auto &Model4x4 = pModel->GetWorldTransformation();
    const auto &Mesh4x4 = pModel->GetMeshTransformation(mMeshName);

    const auto M = Model4x4 * (Mesh4x4 * Node4x4);

    pShader->SetUniformValue("M", M);

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
        pShader->SetUniformValue("N", M.normalMatrix());
        pShader->SetUniformValue("nodeId", static_cast<float>(pModel->GetNodeId()));
        pShader->SetUniformValue("meshId", static_cast<float>(mMeshId));

        pShader->SetUniformValue("hasTextureBaseColor", mMaterial->HasTextureBaseColor());
        pShader->SetUniformValue("hasTextureNormal", mMaterial->HasTextureNormal());

        pShader->SetSampler("textureBaseColor", 0, mMaterial->GetTexture(TextureType::BaseColor));
        pShader->SetSampler("textureNormal", 1, mMaterial->GetTexture(TextureType::Normal));

        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Canavar::Engine::Mesh::AddVertex(const Vertex &vertex)
{
    mVertices.emplace_back(vertex);
}

void Canavar::Engine::Mesh::AddIndex(unsigned int index)
{
    mIndices.push_back(index);
}
