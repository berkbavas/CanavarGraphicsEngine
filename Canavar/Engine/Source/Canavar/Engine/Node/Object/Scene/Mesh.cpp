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
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_DYNAMIC_COPY);

    // Position
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    //Texture Cooords
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Bitangent
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    // Color (required for vertex painter)
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color));
    glEnableVertexAttribArray(5);

    // // Bone IDs
    // glVertexAttribPointer(6, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, boneIDs));
    // glEnableVertexAttribArray(6);

    // // Weights
    // glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, weights));
    // glEnableVertexAttribArray(7);

    // TODO: Check buffers' status here.

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

void Canavar::Engine::Mesh::Render(Model *pModel, Shader *pShader)
{
    if (mVAO == 0)
    {
        LOG_FATAL("Mesh::Render: '{}' is not initialized yet. Returning...", mMeshName.toStdString());
        return;
    }

    const auto &Model4x4 = pModel->GetWorldTransformation();
    const auto &Mesh4x4 = pModel->GetMeshTransformation(mMeshName);

    const auto M = Model4x4 * Mesh4x4;

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

        // Process textures
        bool hasAnyTexture = false;

        pShader->SetUniformValue("useColor", false);
        pShader->SetUniformValue("useTextureAmbient", false);
        pShader->SetUniformValue("useTextureDiffuse", false);
        pShader->SetUniformValue("useTextureSpecular", false);
        pShader->SetUniformValue("useTextureNormal", false);

        if (const auto texture = mMaterial->GetTexture(TextureType::Ambient))
        {
            hasAnyTexture = true;
            pShader->SetUniformValue("useTextureAmbient", true);
            pShader->SetSampler("textureAmbient", 0, texture);
        }

        if (const auto texture = mMaterial->GetTexture(TextureType::Diffuse))
        {
            hasAnyTexture = true;
            pShader->SetUniformValue("useTextureDiffuse", true);
            pShader->SetSampler("textureDiffuse", 1, texture);
        }

        if (const auto texture = mMaterial->GetTexture(TextureType::Specular))
        {
            hasAnyTexture = true;
            pShader->SetUniformValue("useTextureSpecular", true);
            pShader->SetSampler("textureSpecular", 2, texture);
        }

        if (const auto texture = mMaterial->GetTexture(TextureType::Normal))
        {
            pShader->SetUniformValue("useTextureNormal", true);
            pShader->SetSampler("textureNormal", 3, texture);
        }

        if (!hasAnyTexture)
        {
            // If there is no texture, then use color.
            pShader->SetUniformValue("useColor", true);
        }

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
