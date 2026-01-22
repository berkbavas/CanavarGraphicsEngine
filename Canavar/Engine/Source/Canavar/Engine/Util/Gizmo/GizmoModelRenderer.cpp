#include "GizmoModelRenderer.h"

#include "Canavar/Engine/Core/RenderingContext.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"

Canavar::Engine::GizmoModelRenderer::GizmoModelRenderer(RenderingContext *pRenderingContext)
    : mRenderingContext(pRenderingContext)
{
    initializeOpenGLFunctions();
    mCameraManager = mRenderingContext->GetCameraManager();
    mNodeManager = mRenderingContext->GetNodeManager();
    mBasicShader = mRenderingContext->GetShaderManager()->GetShader(ShaderType::Basic);
    mTorusData = std::make_shared<TorusData>(1.0f, 0.05f, 72, 36);
}

void Canavar::Engine::GizmoModelRenderer::Render()
{
    const auto *pCamera = mCameraManager->GetActiveCamera();

    mBasicShader->Bind();
    mBasicShader->SetUniformValue("uFarPlane", pCamera->GetZFar());
    mBasicShader->SetUniformValue("uShadow.Enabled", false);
    mBasicShader->SetUniformValue("uHaze.Enabled", false);
    mBasicShader->SetUniformValue("uCameraPosition", pCamera->GetWorldPosition());
    mBasicShader->SetUniformValue("uViewProjectionMatrix", pCamera->GetViewProjectionMatrix());
    mBasicShader->SetUniformValue("uNumberOfPointLights", 0);
    mBasicShader->SetUniformValue("uNumberOfDirectionalLights", 1);
    mBasicShader->SetUniformValue("uDirectionalLights[0].Direction", -pCamera->GetViewDirection());
    mBasicShader->SetUniformValue("uDirectionalLights[0].Color", QVector3D(1.0f, 1.0f, 1.0f));
    mBasicShader->SetUniformValue("uDirectionalLights[0].Ambient", 1.0f);
    mBasicShader->SetUniformValue("uDirectionalLights[0].Diffuse", 1.0f);
    mBasicShader->SetUniformValue("uDirectionalLights[0].Specular", 1.0f);

    glEnable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);

    for (const auto &pModel : mModels)
    {
        if (pModel->GetVisible() == false)
        {
            continue;
        }

        mBasicShader->SetUniformValue("uModel.Color", pModel->GetColor());
        mBasicShader->SetUniformValue("uModel.Ambient", pModel->GetAmbient());
        mBasicShader->SetUniformValue("uModel.Diffuse", pModel->GetDiffuse());
        mBasicShader->SetUniformValue("uModel.Specular", pModel->GetSpecular());
        mBasicShader->SetUniformValue("uModel.Shininess", pModel->GetShininess());
        mBasicShader->SetUniformValue("uModel.Opacity", 1.0f);
        mBasicShader->SetUniformValue("uNodeId", pModel->GetNodeId());
        mBasicShader->SetUniformValue("uModelMatrix", pModel->GetTransformation());
        mBasicShader->SetUniformValue("uNormalMatrix", pModel->GetLocalNormalMatrix());

        if (const auto *pTorus = dynamic_cast<Torus *>(pModel.get()))
        {
            mTorusData->Render();
        }
    }



    mBasicShader->Release();
}

void Canavar::Engine::GizmoModelRenderer::AddModel(const PrimitiveMeshPtr &pModel)
{
    pModel->SetNodeId(mNodeManager->GetNextNodeId());
    mModels.append(pModel);
}
