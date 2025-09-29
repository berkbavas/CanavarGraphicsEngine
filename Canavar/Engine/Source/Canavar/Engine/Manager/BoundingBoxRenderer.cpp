#include "BoundingBoxRenderer.h"

#include "Canavar/Engine/Core/RenderingContext.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"

void Canavar::Engine::BoundingBoxRenderer::PostInitialize()
{
    const auto* pRenderingContext = GetRenderingContext();

    mNodeManager = pRenderingContext->GetNodeManager();
    mLineShader = pRenderingContext->GetShaderManager()->GetShader(ShaderType::Line);
    mCubeStripData = std::make_shared<CubeStripData>();
}

void Canavar::Engine::BoundingBoxRenderer::InRender(PerspectiveCamera* pCamera)
{
    if (!mDrawBoundingBoxes)
    {
        return;
    }

    const auto& VP = pCamera->GetViewProjectionMatrix();
    const auto& Objects = mNodeManager->GetObjects();

    mLineShader->Bind();
    mLineShader->SetUniformValue("uColor", mLineColor);
    mLineShader->SetUniformValue("uFarPlane", pCamera->GetZFar());

    for (const auto& pObject : Objects)
    {
        if (pObject->GetVisible() == false)
        {
            continue;
        }

        if (ModelPtr pModel = std::dynamic_pointer_cast<Model>(pObject))
        {
            if (const auto pScene = mNodeManager->GetScene(pModel))
            {
                mLineShader->SetUniformValue("uMVP", VP * pModel->GetWorldTransformation() * pScene->GetAABB().GetTransformation());
                mCubeStripData->Render();
            }
            else
            {
                LOG_FATAL("BoundingBoxRenderer::Render: Model data is not found for this model: {}", pModel->GetSceneName());
            }
        }
        else
        {
            mLineShader->SetUniformValue("uMVP", VP * pObject->GetWorldTransformation() * pObject->GetAABB().GetTransformation());
            mCubeStripData->Render();
        }
    }

    mLineShader->Release();
}
