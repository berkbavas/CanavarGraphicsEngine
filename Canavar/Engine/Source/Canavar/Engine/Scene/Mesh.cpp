#include "Mesh.h"

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Model/TexturedModel/TexturedModel.h"
#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::Mesh::Mesh(const QString &MeshName, int MeshId, const QVector<Vertex> &Vertices, const QVector<TriangleFace> &TriangleFaces, TextureMaterialPtr pTextureMaterial)
    : mMeshName(MeshName)
    , mMeshNameStdString(mMeshName.toStdString())
    , mTextureMaterial(pTextureMaterial)
    , mNumIndices(static_cast<unsigned int>(TriangleFaces.size() * 3))
    , mMeshId(MeshId)
{
    LOG_DEBUG("Mesh::Mesh: Initializing '{}'...", mMeshName.toStdString());

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TriangleFaces.size() * sizeof(TriangleFace), TriangleFaces.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // Texture Coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, UV));
    glEnableVertexAttribArray(2);

    // Tangents
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Tangent));
    glEnableVertexAttribArray(3);

    // Bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Bitangent));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);

    LOG_DEBUG("Mesh::Mesh: Initialization of '{}' is done.", mMeshName.toStdString());
}

Canavar::Engine::Mesh::~Mesh()
{
    LOG_DEBUG("Mesh::~Mesh: Destroying '{}'...", mMeshName.toStdString());

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

    LOG_DEBUG("Mesh::~Mesh: Destruction of '{}' is done.", mMeshName.toStdString());
}

void Canavar::Engine::Mesh::Render(TexturedModel *pTexturedModel, Shader *pShader, RenderPass RenderPass, const QMatrix4x4 &NodeTransformation)
{
    const auto MeshOpacity = CalculateMeshOpacity(pTexturedModel->GetOpacity());

    if (!ShouldRenderMesh(RenderPass, MeshOpacity))
    {
        return; // Skip rendering if the mesh should not be rendered for this pass
    }

    const auto ModelMatrix = pTexturedModel->GetTransformation() * NodeTransformation;

    pShader->SetUniform("uModelMatrix", ModelMatrix);
    pShader->SetUniform("uNormalMatrix", ModelMatrix.normalMatrix());
    pShader->SetUniform("uOpacity", MeshOpacity);
    pShader->SetUniform("uMeshId", mMeshId);

    if (const auto pTextureMaterial = mTextureMaterial.lock())
    {
        pShader->SetUniform("uTexture.HasBaseColor", pTextureMaterial->HasTexture(TextureType::BaseColor));
        pShader->SetUniform("uTexture.HasNormal", pTextureMaterial->HasTexture(TextureType::Normal));
        pShader->SetUniform("uTexture.HasMetallic", pTextureMaterial->HasTexture(TextureType::Metallic));
        pShader->SetUniform("uTexture.HasRoughness", pTextureMaterial->HasTexture(TextureType::Roughness));
        pShader->SetUniform("uTexture.HasAmbientOcclusion", pTextureMaterial->HasTexture(TextureType::AmbientOcclusion));

        pShader->SetSampler("uTexture.BaseColor", 0, pTextureMaterial->GetTexture(TextureType::BaseColor));
        pShader->SetSampler("uTexture.Normal", 1, pTextureMaterial->GetTexture(TextureType::Normal));
        pShader->SetSampler("uTexture.Metallic", 2, pTextureMaterial->GetTexture(TextureType::Metallic));
        pShader->SetSampler("uTexture.Roughness", 3, pTextureMaterial->GetTexture(TextureType::Roughness));
        pShader->SetSampler("uTexture.AmbientOcclusion", 4, pTextureMaterial->GetTexture(TextureType::AmbientOcclusion));
    }
    else
    {
        pShader->SetUniform("uTexture.HasBaseColor", false);
        pShader->SetUniform("uTexture.HasNormal", false);
        pShader->SetUniform("uTexture.HasMetallic", false);
        pShader->SetUniform("uTexture.HasRoughness", false);
        pShader->SetUniform("uTexture.HasAmbientOcclusion", false);
    }

    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

const QString &Canavar::Engine::Mesh::GetMeshName() const
{
    return mMeshName;
}

const std::string &Canavar::Engine::Mesh::GetMeshNameStdString() const
{
    return mMeshNameStdString;
}

unsigned int Canavar::Engine::Mesh::GetNumIndices() const
{
    return mNumIndices;
}

Canavar::Engine::TextureMaterialPtr Canavar::Engine::Mesh::GetTextureMaterial() const
{
    return mTextureMaterial.lock();
}

int Canavar::Engine::Mesh::GetMeshId() const
{
    return mMeshId;
}

bool Canavar::Engine::Mesh::ShouldRenderMesh(RenderPass RenderPass, float MeshOpacity) const
{
    switch (RenderPass)
    {
    case Canavar::Engine::RenderPass::Opaque:
        if (MeshOpacity < 1.0f)
        {
            return false; // Skip rendering if the mesh is not fully opaque in the opaque pass
        }
        break;
    case Canavar::Engine::RenderPass::Transparent:
        if (MeshOpacity >= 1.0f)
        {
            return false; // Skip rendering if the mesh is fully opaque in the transparent pass
        }
        break;
    default:
        return false;
    }

    return true; // Render the mesh if it passed all checks
}

float Canavar::Engine::Mesh::CalculateMeshOpacity(float ModelOpacity) const
{
    float Opacity = ModelOpacity;

    // Minimum of textured model and texture material opacity is used to calculate the final mesh opacity.
    if (const auto pTextureMaterial = mTextureMaterial.lock())
    {
        Opacity = std::min(ModelOpacity, pTextureMaterial->GetOpacity()); // Use the minimum of model opacity and texture material opacity
    }

    return Opacity;
}
