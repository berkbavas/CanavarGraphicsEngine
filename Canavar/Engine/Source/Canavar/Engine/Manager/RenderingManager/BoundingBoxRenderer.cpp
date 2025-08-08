#include "BoundingBoxRenderer.h"

#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"

void Canavar::Engine::BoundingBoxRenderer::Initialize()
{
    mCubeStrip = new CubeStrip;
    mLineShader = mShaderManager->GetShader(ShaderType::Line);
}

void Canavar::Engine::BoundingBoxRenderer::Render(Camera* pCamera, float ifps)
{
    const auto& VP = pCamera->GetViewProjectionMatrix();
    const auto& objects = mNodeManager->GetObjects();

    mLineShader->Bind();
    mLineShader->SetUniformValue("uColor", LINE_COLOR);
    mLineShader->SetUniformValue("uZFar", dynamic_cast<PerspectiveCamera*>(pCamera)->GetZFar());

    for (const auto& pObject : objects)
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
                mCubeStrip->Render();
            }
            else
            {
                LOG_FATAL("BoundingBoxRenderer::Render: Model data is not found for this model: {}", pModel->GetSceneName().toStdString());
            }
        }
        else
        {
            mLineShader->SetUniformValue("uMVP", VP * pObject->GetWorldTransformation() * pObject->GetAABB().GetTransformation());
            mCubeStrip->Render();
        }
    }

    mLineShader->Release();
}
