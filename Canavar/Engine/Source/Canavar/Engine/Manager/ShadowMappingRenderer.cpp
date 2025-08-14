#include "ShadowMappingRenderer.h"

#include "Canavar/Engine/Core/ShadowMappingFramebuffer.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Util/Math.h"

void Canavar::Engine::ShadowMappingRenderer::Initialize()
{
    mFramebuffer = new ShadowMappingFramebuffer(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
}

void Canavar::Engine::ShadowMappingRenderer::PostInitialize()
{
    mNodeManager = GetRenderingContext()->GetNodeManager();
    mShadowMappingShader = GetRenderingContext()->GetShaderManager()->GetShader(ShaderType::ShadowMapping);
    mSun = mNodeManager->GetSun();
}

void Canavar::Engine::ShadowMappingRenderer::Render(PerspectiveCamera*)
{
    if (!mShadowsEnabled)
    {
        return;
    }

    CalculateShadowViewProjectionMatrix();
    RenderForShadowMapping();
}

GLuint Canavar::Engine::ShadowMappingRenderer::GetShadowMapTexture() const
{
    return mFramebuffer->GetDepthMap();
}

void Canavar::Engine::ShadowMappingRenderer::CalculateShadowViewProjectionMatrix()
{
    mLightProjectionMatrix.setToIdentity();

    if (mUseOrthographicProjection)
    {
        mLightProjectionMatrix.ortho(-mOrthographicSize, mOrthographicSize, -mOrthographicSize, mOrthographicSize, -mOrthographicSize, mOrthographicSize);
    }
    else
    {
        mLightProjectionMatrix.perspective(mFov, 1, mZNear, mZFar);
    }

    mLightViewMatrix.setToIdentity();
    mLightViewMatrix.lookAt(-mSunDistance * mSun->GetDirection(), QVector3D(0, 0, 0), QVector3D(0, -1, 0));

    mLightViewProjectionMatrix = mLightProjectionMatrix * mLightViewMatrix;
}

void Canavar::Engine::ShadowMappingRenderer::RenderForShadowMapping()
{
    mFramebuffer->Bind();
    mShadowMappingShader->Bind();
    mShadowMappingShader->SetUniformValue("uLightViewProjectionMatrix", mLightViewProjectionMatrix);

    const auto& Nodes = mNodeManager->GetNodes();

    for (const auto& pNode : Nodes)
    {
        if (const auto pObject = std::dynamic_pointer_cast<Object>(pNode))
        {
            if (pObject->GetVisible() == false)
            {
                continue;
            }

            if (ModelPtr pModel = std::dynamic_pointer_cast<Model>(pObject))
            {
                mShadowMappingShader->SetUniformValue("uModelMatrix", pModel->GetWorldTransformation());

                if (const auto pScene = mNodeManager->GetScene(pModel))
                {
                    pScene->Render(pModel.get(), mShadowMappingShader, RenderPass::Opaque);
                }
            }
        }
    }

    mShadowMappingShader->Release();
}
