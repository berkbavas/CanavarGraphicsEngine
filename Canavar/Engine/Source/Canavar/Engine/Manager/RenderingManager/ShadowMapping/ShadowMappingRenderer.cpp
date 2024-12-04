#include "ShadowMappingRenderer.h"

#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/ShadowMapping/ShadowMappingFramebuffer.h"
#include "Canavar/Engine/Manager/ShaderManager.h"

void Canavar::Engine::ShadowMappingRenderer::Initialize()
{
    initializeOpenGLFunctions();

    mFramebuffer = new ShadowMappingFramebuffer(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    mShadowMappingShader = mShaderManager->GetShader(ShaderType::ShadowMapping);
}

void Canavar::Engine::ShadowMappingRenderer::Render(float ifps)
{
    constexpr QVector3D ORIGIN(0, 0, 0);
    constexpr QVector3D UP(0, 1, 0);
    
    mLightViewMatrix.setToIdentity();
    mLightViewMatrix.lookAt(-mSunDistance * mSun->GetDirection(), ORIGIN, UP);

    mLightProjectionMatrix.setToIdentity();
    mLightProjectionMatrix.perspective(mFov, 1, mZNear, mZFar);

    mLightViewProjectionMatrix = mLightProjectionMatrix * mLightViewMatrix;

    mFramebuffer->Bind();
    mShadowMappingShader->Bind();

    mShadowMappingShader->SetUniformValue("LVP", mLightViewProjectionMatrix);

    const auto& nodes = mNodeManager->GetNodes();

    for (const auto& pNode : nodes)
    {
        if (const auto pObject = std::dynamic_pointer_cast<Object>(pNode))
        {
            if (pObject->GetVisible() == false)
            {
                continue;
            }

            if (ModelPtr pModel = std::dynamic_pointer_cast<Model>(pObject))
            {
                if (const auto pScene = mNodeManager->GetScene(pModel))
                {
                    pScene->Render(pModel.get(), mShadowMappingShader);
                }
            }
        }
    }

    mShadowMappingShader->Release();
}

GLuint Canavar::Engine::ShadowMappingRenderer::GetShadowMap() const
{
    return mFramebuffer->GetDepthMap();
}
