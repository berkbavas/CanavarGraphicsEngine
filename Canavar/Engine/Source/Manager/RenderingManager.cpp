#include "RenderingManager.h"

#include "Manager/CameraManager.h"
#include "Manager/LightManager.h"
#include "Manager/NodeManager.h"
#include "Manager/ShaderManager.h"

Canavar::Engine::RenderingManager::RenderingManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::RenderingManager::Initialize()
{
    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    mFramebufferFormats[Default].setSamples(8);
    mFramebufferFormats[Default].setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    mFramebufferFormats[Default].setInternalTextureFormat(GL_RGBA32F);

    mFramebufferFormats[Temp].setSamples(0);
    mFramebufferFormats[Temp].setAttachment(QOpenGLFramebufferObject::Attachment::NoAttachment);
    mFramebufferFormats[Temp].setInternalTextureFormat(GL_RGBA32F);

    for (const auto type : { Default, Temp })
    {
        mFramebuffers[type] = nullptr;
    }

    ResizeFramebuffers();
}

void Canavar::Engine::RenderingManager::PostInitialize()
{
    mShaderManager = mManagerProvider->GetShaderManager();
    mNodeManager = mManagerProvider->GetNodeManager();
    mCameraManager = mManagerProvider->GetCameraManager();
    mLightManager = mManagerProvider->GetLightManager();

    mModelShader = mShaderManager->GetShader(ShaderType::Model);
}

void Canavar::Engine::RenderingManager::Render(float ifps)
{
    mActiveCamera = mCameraManager->GetActiveCamera();

    mFramebuffers[Default]->bind();
    glViewport(0, 0, mActiveCamera->GetWidth(), mActiveCamera->GetHeight());
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // QOpenGLFramebufferObject::bindDefault();
    // glViewport(0, 0, mActiveCamera->GetWidth(), mActiveCamera->GetHeight());
    // glClearColor(0, 0, 0, 1);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SetCommonUniforms(mModelShader);

    const auto& models = mNodeManager->GetModels();

    for (const auto& pModel : models)
    {
        if (!pModel->GetEnabled())
        {
            continue;
        }

        RenderModel(pModel);
    }

    QOpenGLFramebufferObject::bindDefault();
    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QOpenGLFramebufferObject::blitFramebuffer(nullptr, mFramebuffers[Default]);

    // // Blit to temp
    // for (int index = 0; index < 2; index++)
    // {
    //     QOpenGLFramebufferObject::blitFramebuffer(
    //         mFramebuffers[Temp],
    //         QRect(0, 0, mFramebuffers[Temp]->width(), mFramebuffers[Temp]->height()),
    //         mFramebuffers[Default],
    //         QRect(0, 0, mFramebuffers[Default]->width(), mFramebuffers[Default]->height()),
    //         GL_COLOR_BUFFER_BIT,
    //         GL_LINEAR,
    //         index,
    //         index);
    // }

    // mShaderManager->Bind(ShaderType::PostProcessShader);
    // mShaderManager->SetSampler("sceneTexture", 0, mFBOs[FramebufferType::Temporary]->texture());
    // mShaderManager->SetSampler("bloomBlurTexture", 1, mFBOs[qMax(0, mBlurPass) % 2 == 0 ? FramebufferType::Ping : FramebufferType::Pong]->texture());
    // mShaderManager->SetUniformValue("exposure", mExposure);
    // mShaderManager->SetUniformValue("gamma", mGamma);
    // glBindVertexArray(mQuad.mVAO);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // mShaderManager->Release();
}

void Canavar::Engine::RenderingManager::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    ResizeFramebuffers();
}

void Canavar::Engine::RenderingManager::RenderModel(ModelPtr pModel)
{
    const auto& pointLights = mLightManager->GetPointLightsAround(pModel->GetPosition(), 100.0f);

    const auto numberOfPointLights = std::min(8, static_cast<int>(pointLights.size()));

    mModelShader->Bind();

    mModelShader->SetUniformValue("M", pModel->GetTransformation());
    mModelShader->SetUniformValue("N", pModel->GetNormalMatrix());

    mModelShader->SetUniformValue("model.color", pModel->GetColor());
    mModelShader->SetUniformValue("model.ambient", pModel->GetAmbient());
    mModelShader->SetUniformValue("model.diffuse", pModel->GetDiffuse());
    mModelShader->SetUniformValue("model.specular", pModel->GetSpecular());
    mModelShader->SetUniformValue("model.shininess", pModel->GetShininess());

    mModelShader->SetUniformValue("numberOfPointLights", numberOfPointLights);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        mModelShader->SetUniformValue("pointLights[" + QString::number(i) + "].color", pointLights[i]->GetColor());
        mModelShader->SetUniformValue("pointLights[" + QString::number(i) + "].position", pointLights[i]->GetPosition());
        mModelShader->SetUniformValue("pointLights[" + QString::number(i) + "].ambient", pointLights[i]->GetAmbient());
        mModelShader->SetUniformValue("pointLights[" + QString::number(i) + "].diffuse", pointLights[i]->GetDiffuse());
        mModelShader->SetUniformValue("pointLights[" + QString::number(i) + "].specular", pointLights[i]->GetSpecular());
        mModelShader->SetUniformValue("pointLights[" + QString::number(i) + "].constant", pointLights[i]->GetConstant());
        mModelShader->SetUniformValue("pointLights[" + QString::number(i) + "].linear", pointLights[i]->GetLinear());
        mModelShader->SetUniformValue("pointLights[" + QString::number(i) + "].quadratic", pointLights[i]->GetQuadratic());
    }

    mNodeManager->GetScene(pModel)->Render(pModel.get(), mModelShader);

    mModelShader->Release();
}

void Canavar::Engine::RenderingManager::SetCommonUniforms(Shader* pShader)
{
    const auto& lights = mLightManager->GetDirectionalLights();
    const auto numberOfLights = std::min(8, static_cast<int>(lights.size()));

    pShader->Bind();
    pShader->SetUniformValue("haze.enabled", false);
    pShader->SetUniformValue("VP", mActiveCamera->GetViewProjectionMatrix());

    pShader->SetUniformValue("numberOfDirectionalLights", numberOfLights);

    for (int i = 0; i < numberOfLights; i++)
    {
        pShader->SetUniformValue("directionalLights[" + QString::number(i) + "].direction", lights[i]->GetDirection());
        pShader->SetUniformValue("directionalLights[" + QString::number(i) + "].color", lights[i]->GetColor());
        pShader->SetUniformValue("directionalLights[" + QString::number(i) + "].ambient", lights[i]->GetAmbient());
        pShader->SetUniformValue("directionalLights[" + QString::number(i) + "].diffuse", lights[i]->GetDiffuse());
        pShader->SetUniformValue("directionalLights[" + QString::number(i) + "].specular", lights[i]->GetSpecular());
    }

    pShader->Release();
}

void Canavar::Engine::RenderingManager::ResizeFramebuffers()
{
    for (const auto type : { Default, Temp })
    {
        if (mFramebuffers[type])
        {
            delete mFramebuffers[type];
            mFramebuffers[type] = nullptr;
        }
    }

    constexpr GLuint ATTACHMENTS[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    for (const auto type : { Default, Temp })
    {
        mFramebuffers[type] = new QOpenGLFramebufferObject(mWidth, mHeight, mFramebufferFormats[type]);

        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGB32F);
        mFramebuffers[type]->bind();
        glDrawBuffers(2, ATTACHMENTS);
        mFramebuffers[type]->release();
    }
}
