#include "TexturedModel.h"

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Scene/Scene.h"

Canavar::Engine::TexturedModel::TexturedModel(const QString &ModelName)
    : Object()
    , mModelName(ModelName)
    , mModelUniqueNameStdString(ModelName.toStdString())
{
    SetNodeName(mModelName.toStdString());
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
