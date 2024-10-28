#include "BoundingBoxRenderer.h"

#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"

void Canavar::Engine::BoundingBoxRenderer::Initialize()
{
    mCubeStrip = new CubeStrip;
    mLineShader = mShaderManager->GetShader(ShaderType::Line);
}

void Canavar::Engine::BoundingBoxRenderer::Render(float ifps)
{
    mActiveCamera = mCameraManager->GetActiveCamera();

    const auto& VP = mActiveCamera->GetViewProjectionMatrix();
    const auto& nodes = mNodeManager->GetNodes();

    mLineShader->Bind();
    mLineShader->SetUniformValue("color", LINE_COLOR);

    for (const auto& pNode : nodes)
    {
        if (!pNode->GetVisible())
        {
            continue;
        }

        if (!pNode->GetSelectable())
        {
            continue;
        }

        if (ModelPtr pModel = std::dynamic_pointer_cast<Model>(pNode))
        {
            if (const auto pScene = mNodeManager->GetScene(pModel))
            {
                mLineShader->SetUniformValue("MVP", VP * pModel->GetWorldTransformation() * pScene->GetAABB().GetTransformation());
                mCubeStrip->Render();
            }
            else
            {
                LOG_FATAL("BoundingBoxRenderer::Render: Model data is not found for this model: {}", pModel->GetModelName().toStdString());
            }
        }
        else
        {
            mLineShader->SetUniformValue("MVP", VP * pNode->GetWorldTransformation() * pNode->GetAABB().GetTransformation());
            mCubeStrip->Render();
        }
    }

    mLineShader->Release();
}
