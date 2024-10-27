#include "Mesh.h"

#include "Canavar/Engine/Util/Logger.h"

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
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    // TODO: Check buffers' status here.

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

    // Bone IDs
    glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, boneIDs));
    glEnableVertexAttribArray(5);

    // Weights
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, weights));
    glEnableVertexAttribArray(6);

    glBindVertexArray(0);

    LOG_DEBUG("Mesh::Initialize: Initialization of '{}' is done.", mMeshName.toStdString());
}

void Canavar::Engine::Mesh::Destroy()
{
    // TODO
}

void Canavar::Engine::Mesh::Render(Model *pModel, Shader *pShader)
{
    if (mVAO == 0)
    {
        LOG_WARN("Mesh::Render: '{}' is not initialized.", mMeshName.toStdString());
        return;
    }

    pShader->SetUniformValue("M", pModel->GetWorldTransformation() * pModel->GetMeshTransformation(mMeshName));
    pShader->SetUniformValue("N", pModel->GetWorldTransformation().normalMatrix() * pModel->GetMeshTransformation(mMeshName).normalMatrix());

    // Process textures
    bool hasAnyTexture = false;

    pShader->SetUniformValue("useColor", false);
    pShader->SetUniformValue("useTextureAmbient", false);
    pShader->SetUniformValue("useTextureDiffuse", false);
    pShader->SetUniformValue("useTextureSpecular", false);
    pShader->SetUniformValue("useTextureNormal", false);

    if (const auto pTexture = mMaterial->GetTexture(TextureType::Ambient))
    {
        hasAnyTexture = true;
        pShader->SetUniformValue("useTextureAmbient", true);
        pShader->SetSampler("textureAmbient", 0, pTexture->textureId());
    }
    else if (const auto pTexture = mMaterial->GetTexture(TextureType::Diffuse))
    {
        // Use diffuse texture in place of ambient if there is no ambient texture.
        hasAnyTexture = true;
        pShader->SetUniformValue("useTextureAmbient", true);
        pShader->SetSampler("textureAmbient", 0, pTexture->textureId());
    }

    if (const auto pTexture = mMaterial->GetTexture(TextureType::Diffuse))
    {
        hasAnyTexture = true;
        pShader->SetUniformValue("useTextureDiffuse", true);
        pShader->SetSampler("textureDiffuse", 1, pTexture->textureId());
    }

    if (const auto pTexture = mMaterial->GetTexture(TextureType::Specular))
    {
        hasAnyTexture = true;
        pShader->SetUniformValue("useTextureSpecular", true);
        pShader->SetSampler("textureSpecular", 2, pTexture->textureId());
    }

    if (const auto pTexture = mMaterial->GetTexture(TextureType::Normal))
    {
        pShader->SetUniformValue("useTextureNormal", true);
        pShader->SetSampler("textureNormal", 3, pTexture->textureId());
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

void Canavar::Engine::Mesh::AddVertex(const Vertex &vertex)
{
    mVertices.emplace_back(vertex);
}

void Canavar::Engine::Mesh::AddIndex(unsigned int index)
{
    mIndices.push_back(index);
}
