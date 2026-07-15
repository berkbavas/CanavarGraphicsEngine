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

void Canavar::Engine::TexturedModel::Render(Scene *pScene, Shader *pShader, const RenderPassParameters &RenderPassParameters, const QVector<PointLight *> &PointLights)
{
    const auto NumPointLights = std::min(static_cast<int>(PointLights.size()), MAX_POINT_LIGHTS);

    pShader->Bind();
    pShader->SetUniform("uNumPointLights", NumPointLights);

    for (int i = 0; i < NumPointLights; ++i)
    {
        const auto &pPointLight = PointLights[i];

        pShader->SetUniform(QString("uPointLights[%1].Color").arg(i), pPointLight->GetColor());
        pShader->SetUniform(QString("uPointLights[%1].Position").arg(i), pPointLight->GetPosition());
        pShader->SetUniform(QString("uPointLights[%1].Ambient").arg(i), pPointLight->GetAmbient());
        pShader->SetUniform(QString("uPointLights[%1].Diffuse").arg(i), pPointLight->GetDiffuse());
        pShader->SetUniform(QString("uPointLights[%1].Specular").arg(i), pPointLight->GetSpecular());
        pShader->SetUniform(QString("uPointLights[%1].Constant").arg(i), pPointLight->GetConstant());
        pShader->SetUniform(QString("uPointLights[%1].Linear").arg(i), pPointLight->GetLinear());
        pShader->SetUniform(QString("uPointLights[%1].Quadratic").arg(i), pPointLight->GetQuadratic());
    }

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

    pScene->Render(this, pShader, RenderPassParameters.RenderPass);

    pShader->Unbind();
}
