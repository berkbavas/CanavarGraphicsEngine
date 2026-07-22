#include "TexturedModel.h"

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Scene/Scene.h"

const QMatrix4x4 Canavar::Engine::TexturedModel::sIdentityMatrix;

Canavar::Engine::TexturedModel::TexturedModel(const QString &ModelName)
    : Object()
    , mModelName(ModelName)
    , mModelUniqueNameStdString(ModelName.toStdString())
{
    SetNodeName(mModelName.toStdString());
}

// ── Per-mesh opacity overrides ────────────────────────────────────────────────

float Canavar::Engine::TexturedModel::GetMeshOpacityOverride(int MeshId) const
{
    const auto It = mMeshOpacityOverrides.constFind(MeshId);
    return (It != mMeshOpacityOverrides.constEnd()) ? *It : 1.0f;
}

void Canavar::Engine::TexturedModel::SetMeshOpacityOverride(int MeshId, float Opacity)
{
    mMeshOpacityOverrides[MeshId] = Opacity;
}

bool Canavar::Engine::TexturedModel::HasMeshOpacityOverride(int MeshId) const
{
    return mMeshOpacityOverrides.contains(MeshId);
}

// ── Per-mesh transform overrides ──────────────────────────────────────────────

const QMatrix4x4 &Canavar::Engine::TexturedModel::GetMeshTransformOverride(int MeshId) const
{
    const auto It = mMeshTransformOverrides.constFind(MeshId);
    return (It != mMeshTransformOverrides.constEnd()) ? *It : sIdentityMatrix;
}

void Canavar::Engine::TexturedModel::SetMeshTransformOverride(int MeshId, const QMatrix4x4 &Transform)
{
    mMeshTransformOverrides[MeshId] = Transform;
}

bool Canavar::Engine::TexturedModel::HasMeshTransformOverride(int MeshId) const
{
    return mMeshTransformOverrides.contains(MeshId);
}

bool Canavar::Engine::TexturedModel::IsFuzzyTransparent() const
{
    return mOpacity < EPSILON;
}

void Canavar::Engine::TexturedModel::Render(Scene *pScene, Shader *pShader, RenderPass RenderPass)
{
    pShader->Bind();
    pShader->SetUniform("uPhongMaterial.Ambient", mPhongMaterial.Ambient);
    pShader->SetUniform("uPhongMaterial.Diffuse", mPhongMaterial.Diffuse);
    pShader->SetUniform("uPhongMaterial.Specular", mPhongMaterial.Specular);
    pShader->SetUniform("uPhongMaterial.Shininess", mPhongMaterial.Shininess);
    pShader->SetUniform("uPbrMaterial.Metallic", mPbrMaterial.Metallic);
    pShader->SetUniform("uPbrMaterial.Roughness", mPbrMaterial.Roughness);
    pShader->SetUniform("uPbrMaterial.AmbientOcclusion", mPbrMaterial.AmbientOcclusion);
    pShader->SetUniform("uColor", mColor);
    pShader->SetUniform("uUseColor", mUseColor);
    pShader->SetUniform("uShadingMode", static_cast<int>(mShadingMode));
    pShader->SetUniform("uNodeId", GetNodeId());
    pScene->Render(this, pShader, RenderPass);
    pShader->Unbind();
}
