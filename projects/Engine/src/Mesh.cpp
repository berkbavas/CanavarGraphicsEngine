#include "Mesh.h"
#include "CameraManager.h"
#include "Common.h"
#include "Model.h"
#include "ShaderManager.h"

Canavar::Engine::Mesh::Mesh()
    : QObject()
    , mVAO(nullptr)
    , mMaterial(nullptr)
{
    mShaderManager = ShaderManager::Instance();
    mCameraManager = CameraManager::Instance();
}

Canavar::Engine::Mesh::~Mesh()
{
    // TODO
}

void Canavar::Engine::Mesh::AddVertex(const Vertex& vertex)
{
    mVertices << vertex;
}

void Canavar::Engine::Mesh::AddIndex(unsigned int index)
{
    mIndices << index;
}

void Canavar::Engine::Mesh::SetMaterial(Material* material)
{
    mMaterial = material;
}

void Canavar::Engine::Mesh::Create()
{
    initializeOpenGLFunctions();
    mVAO = new QOpenGLVertexArrayObject;
    mVAO->create();
    mVAO->bind();

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.constData(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    //Texture Cooords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    // IDs
    glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
    glEnableVertexAttribArray(5);

    // Weights
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
    glEnableVertexAttribArray(6);

    mVAO->release();

    // Vertex Rendering

    mVerticesVAO = new QOpenGLVertexArrayObject;
    mVerticesVAO->create();
    mVerticesVAO->bind();

    glGenBuffers(1, &mVerticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE), CUBE, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    mVerticesVAO->release();
}

void Canavar::Engine::Mesh::Render(RenderModes modes, Model* model)
{
    if (modes.testFlag(RenderMode::Custom))
    {
        mVAO->bind();
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        mVAO->release();
    }

    if (modes.testFlag(RenderMode::Raycaster))
    {
        mShaderManager->SetUniformValue("M", model->WorldTransformation() * model->GetMeshTransformation(mName));

        mVAO->bind();
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        mVAO->release();
    }

    if (modes.testFlag(RenderMode::Default))
    {
        if (bool useTexture = mMaterial->GetNumberOfTextures())
        {
            mShaderManager->Bind(ShaderType::ModelTexturedShader);
            mShaderManager->SetUniformValue("N", model->WorldTransformation().normalMatrix() * model->GetMeshTransformation(mName).normalMatrix());
            mShaderManager->SetUniformValue("useTextureAmbient", false);
            mShaderManager->SetUniformValue("useTextureDiffuse", false);
            mShaderManager->SetUniformValue("useTextureSpecular", false);
            mShaderManager->SetUniformValue("useTextureNormal", false);

            if (auto texture = mMaterial->Get(Material::TextureType::Ambient))
            {
                mShaderManager->SetUniformValue("useTextureAmbient", true);
                mShaderManager->SetSampler("textureAmbient", 0, texture->textureId());
            }
            else if (auto texture = mMaterial->Get(Material::TextureType::Diffuse))
            {
                mShaderManager->SetUniformValue("useTextureAmbient", true);
                mShaderManager->SetSampler("textureAmbient", 0, texture->textureId()); // Use diffuse texture if there is no ambient texture
            }

            if (auto texture = mMaterial->Get(Material::TextureType::Diffuse))
            {
                mShaderManager->SetUniformValue("useTextureDiffuse", true);
                mShaderManager->SetSampler("textureDiffuse", 1, texture->textureId());
            }

            if (auto texture = mMaterial->Get(Material::TextureType::Specular))
            {
                mShaderManager->SetUniformValue("useTextureSpecular", true);
                mShaderManager->SetSampler("textureSpecular", 2, texture->textureId());
            }

            if (auto texture = mMaterial->Get(Material::TextureType::Normal))
            {
                mShaderManager->SetUniformValue("useTextureNormal", true);
                mShaderManager->SetSampler("textureNormal", 3, texture->textureId());
            }
        }
        else
        {
            mShaderManager->Bind(ShaderType::ModelColoredShader);
            mShaderManager->SetUniformValue("model.color", model->GetColor());
        }

        // Common uniforms
        mShaderManager->SetUniformValue("M", model->WorldTransformation() * model->GetMeshTransformation(mName));
        mShaderManager->SetUniformValue("model.overlayColor", model->GetOverlayColor());
        mShaderManager->SetUniformValue("model.overlayColorFactor", model->GetOverlayColorFactor());
        mShaderManager->SetUniformValue("model.meshOverlayColor", model->GetMeshOverlayColor(mName));
        mShaderManager->SetUniformValue("model.meshOverlayColorFactor", model->GetMeshOverlayColorFactor(mName));
        mShaderManager->SetUniformValue("model.shininess", model->GetShininess());
        mShaderManager->SetUniformValue("model.ambient", model->GetAmbient());
        mShaderManager->SetUniformValue("model.diffuse", model->GetDiffuse());
        mShaderManager->SetUniformValue("model.specular", model->GetSpecular());

        mVAO->bind();
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        mVAO->release();

        mShaderManager->Release();
    }

    if (modes.testFlag(RenderMode::NodeInfo))
    {
        mShaderManager->Bind(ShaderType::NodeInfoShader);
        mShaderManager->SetUniformValue("MVP", mCameraManager->GetActiveCamera()->GetViewProjectionMatrix() * model->WorldTransformation() * model->GetMeshTransformation(mName));
        mShaderManager->SetUniformValue("nodeID", model->GetID());
        mShaderManager->SetUniformValue("meshID", mID);
        mShaderManager->SetUniformValue("fillVertexInfo", false);

        mVAO->bind();
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        mVAO->release();

        mShaderManager->Release();
    }
}

Canavar::Engine::Mesh::Vertex Canavar::Engine::Mesh::GetVertex(int index) const
{
    Vertex vertex;

    if (0 <= index && index < mVertices.size())
        vertex = mVertices.at(index);

    return vertex;
}

int Canavar::Engine::Mesh::GetNumberOfVertices()
{
    return mVertices.size();
}

QOpenGLVertexArrayObject* Canavar::Engine::Mesh::GetVerticesVAO() const
{
    return mVerticesVAO;
}