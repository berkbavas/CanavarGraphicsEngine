#include "RenderingManager.h"

#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/BoundingBoxRenderer.h"
#include "Canavar/Engine/Manager/ShaderManager.h"

Canavar::Engine::RenderingManager::RenderingManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::RenderingManager::Initialize()
{
    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    mFramebufferFormats[Default].setSamples(4);
    mFramebufferFormats[Default].setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    mFramebufferFormats[Temp].setSamples(0);
    mFramebufferFormats[Ping].setSamples(0);
    mFramebufferFormats[Pong].setSamples(0);

    for (const auto type : { Default, Temp, Ping, Pong })
    {
        mFramebuffers[type] = nullptr;
    }

    mQuad = new Quad;

    mBoundingBoxRenderer = new BoundingBoxRenderer;

    ResizeFramebuffers();
}

void Canavar::Engine::RenderingManager::PostInitialize()
{
    mShaderManager = mManagerProvider->GetShaderManager();
    mNodeManager = mManagerProvider->GetNodeManager();
    mCameraManager = mManagerProvider->GetCameraManager();
    mLightManager = mManagerProvider->GetLightManager();

    mBoundingBoxRenderer->SetCameraManager(mCameraManager);
    mBoundingBoxRenderer->SetShaderManager(mShaderManager);
    mBoundingBoxRenderer->SetNodeManager(mNodeManager);

    mBoundingBoxRenderer->Initialize();

    mSky = mNodeManager->GetSky();
    mSun = mLightManager->GetSun();
    mTerrain = mNodeManager->GetTerrain();
    mHaze = mNodeManager->GetHaze();

    mModelShader = mShaderManager->GetShader(ShaderType::Model);
    mSkyShader = mShaderManager->GetShader(ShaderType::Sky);
    mTerrainShader = mShaderManager->GetShader(ShaderType::Terrain);
    mBlurShader = mShaderManager->GetShader(ShaderType::Blur);
    mPostProcessShader = mShaderManager->GetShader(ShaderType::PostProcess);
    mNozzleEffectShader = mShaderManager->GetShader(ShaderType::NozzleEffect);
}

void Canavar::Engine::RenderingManager::Render(float ifps)
{
    // ----------------------- RENDER LOOP BEGINS -------------------

    mActiveCamera = mCameraManager->GetActiveCamera();

    mFramebuffers[Default]->bind();
    glViewport(0, 0, mActiveCamera->GetWidth(), mActiveCamera->GetHeight());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SetUniforms();

    mSky->Render(mSkyShader, mSun.get(), mActiveCamera.get());
    mTerrain->Render(mTerrainShader, mActiveCamera.get());

    const auto& models = mNodeManager->GetModels();

    for (const auto& pModel : models)
    {
        if (pModel->GetVisible())
        {
            RenderModel(pModel);
        }
    }

    const auto& nodes = mNodeManager->GetNodes();

    for (const auto& pNode : nodes)
    {
        if (NozzleEffectPtr pEffect = std::dynamic_pointer_cast<NozzleEffect>(pNode))
        {
            if (pEffect->GetVisible())
            {
                RenderNozzleEffect(pEffect, ifps);
            }
        }
    }

    if (mDrawBoundingBoxes)
    {
        mBoundingBoxRenderer->Render(ifps);
    }

    // ----------------------- RENDER LOOP ENDS -------------------

    // ----------------------- POST PROCESSING BEGINS -------------------

    // Default -> Ping
    QOpenGLFramebufferObject::blitFramebuffer( //
        mFramebuffers[Ping],
        QRect(0, 0, mWidth, mHeight),
        mFramebuffers[Default],
        QRect(0, 0, mWidth, mHeight),
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST,
        1, // <- Blur location of Default FBO
        0  // <- Write location of Ping FBO
    );

    // Ping -> Pong -> Ping -> ... Pxng
    for (int i = 0; i < qMax(0, mBlurPass); i++)
    {
        mFramebuffers[i % 2 == 0 ? Pong : Ping]->bind();
        mBlurShader->Bind();
        mBlurShader->SetUniformValue("horizontal", i % 2 == 0);
        mBlurShader->SetSampler("targetTexture", 0, mFramebuffers[i % 2 == 0 ? Ping : Pong]->texture());
        mQuad->Render();
        mBlurShader->Release();
    }

    for (int index = 0; index <= 2; index++)
    {
        QOpenGLFramebufferObject::blitFramebuffer( //
            mFramebuffers[Temp],
            QRect(0, 0, mFramebuffers[Temp]->width(), mFramebuffers[Temp]->height()),
            mFramebuffers[Default],
            QRect(0, 0, mFramebuffers[Default]->width(), mFramebuffers[Default]->height()),
            GL_COLOR_BUFFER_BIT,
            GL_LINEAR,
            index,
            index);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mActiveCamera->GetWidth(), mActiveCamera->GetHeight());
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mPostProcessShader->Bind();
    mPostProcessShader->SetSampler("sceneTexture", 0, mFramebuffers[Temp]->textures().at(0));
    mPostProcessShader->SetSampler("bloomTexture", 1, mFramebuffers[qMax(0, mBlurPass) % 2 == 0 ? Ping : Pong]->texture());
    mQuad->Render();
    mPostProcessShader->Release();
}

void Canavar::Engine::RenderingManager::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    ResizeFramebuffers();
}

QVector3D Canavar::Engine::RenderingManager::GetMouseFragmentLocalPosition(int x, int y)
{
    QVector3D position;
    mFramebuffers[Temp]->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glReadPixels(mDevicePixelRatio * x, mFramebuffers[Temp]->height() - mDevicePixelRatio * y, 1, 1, GL_RGBA, GL_FLOAT, &position);
    mFramebuffers[Temp]->release();
    return position;
}

void Canavar::Engine::RenderingManager::RenderModel(ModelPtr pModel)
{
    SetPointLights(mModelShader, pModel);

    const auto& M = pModel->GetWorldTransformation();
    mModelShader->Bind();
    mModelShader->SetUniformValue("M", M);
    mModelShader->SetUniformValue("N", M.normalMatrix());
    mModelShader->SetUniformValue("model.color", pModel->GetColor());
    mModelShader->SetUniformValue("model.ambient", pModel->GetAmbient());
    mModelShader->SetUniformValue("model.diffuse", pModel->GetDiffuse());
    mModelShader->SetUniformValue("model.specular", pModel->GetSpecular());
    mModelShader->SetUniformValue("model.shininess", pModel->GetShininess());

    if (const auto pScene = mNodeManager->GetScene(pModel))
    {
        pScene->Render(pModel.get(), mModelShader);
    }
    else
    {
        LOG_FATAL("RenderingManager::RenderModel: Model data is not found for this model: {}", pModel->GetModelName().toStdString());
    }

    mModelShader->Release();
}

void Canavar::Engine::RenderingManager::RenderNozzleEffect(NozzleEffectPtr pEffect, float ifps)
{
    mNozzleEffectShader->Bind();
    mNozzleEffectShader->SetUniformValue("MVP", mActiveCamera->GetViewProjectionMatrix() * pEffect->GetWorldTransformation());
    mNozzleEffectShader->SetUniformValue("scale", pEffect->GetScale());
    mNozzleEffectShader->SetUniformValue("maxRadius", pEffect->GetMaxRadius());
    mNozzleEffectShader->SetUniformValue("maxDistance", pEffect->GetMaxDistance());
    mNozzleEffectShader->SetUniformValue("speed", pEffect->GetSpeed());
    pEffect->Render(ifps);
    mNozzleEffectShader->Release();
}

void Canavar::Engine::RenderingManager::SetUniforms()
{
    SetCommonUniforms(mModelShader);
    SetCommonUniforms(mTerrainShader);

    SetDirectionalLights(mModelShader);

    SetDirectionalLights(mTerrainShader);
    SetPointLights(mTerrainShader, mActiveCamera);
}

void Canavar::Engine::RenderingManager::SetCommonUniforms(Shader* pShader)
{
    pShader->Bind();
    pShader->SetUniformValue("haze.enabled", mHaze->GetEnabled());
    pShader->SetUniformValue("haze.color", mHaze->GetColor());
    pShader->SetUniformValue("haze.density", mHaze->GetDensity());
    pShader->SetUniformValue("haze.gradient", mHaze->GetGradient());
    pShader->SetUniformValue("cameraPosition", mActiveCamera->GetWorldPosition());
    pShader->SetUniformValue("VP", mActiveCamera->GetViewProjectionMatrix());
    pShader->Release();
}

void Canavar::Engine::RenderingManager::SetDirectionalLights(Shader* pShader)
{
    const auto& lights = mLightManager->GetDirectionalLights();
    const auto numberOfLights = std::min(8, static_cast<int>(lights.size()));

    pShader->Bind();
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

void Canavar::Engine::RenderingManager::SetPointLights(Shader* pShader, NodePtr pNode)
{
    const auto& lights = mLightManager->GetPointLightsAround(pNode->GetWorldPosition(), 100'000.0f);
    const auto numberOfPointLights = std::min(8, static_cast<int>(lights.size()));

    pShader->Bind();
    pShader->SetUniformValue("numberOfPointLights", numberOfPointLights);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        pShader->SetUniformValue("pointLights[" + QString::number(i) + "].color", lights[i]->GetColor());
        pShader->SetUniformValue("pointLights[" + QString::number(i) + "].position", lights[i]->GetWorldPosition());
        pShader->SetUniformValue("pointLights[" + QString::number(i) + "].ambient", lights[i]->GetAmbient());
        pShader->SetUniformValue("pointLights[" + QString::number(i) + "].diffuse", lights[i]->GetDiffuse());
        pShader->SetUniformValue("pointLights[" + QString::number(i) + "].specular", lights[i]->GetSpecular());
        pShader->SetUniformValue("pointLights[" + QString::number(i) + "].constant", lights[i]->GetConstant());
        pShader->SetUniformValue("pointLights[" + QString::number(i) + "].linear", lights[i]->GetLinear());
        pShader->SetUniformValue("pointLights[" + QString::number(i) + "].quadratic", lights[i]->GetQuadratic());
    }

    pShader->Release();
}

void Canavar::Engine::RenderingManager::ResizeFramebuffers()
{
    for (const auto type : { Default, Temp, Ping, Pong })
    {
        if (mFramebuffers[type])
        {
            delete mFramebuffers[type];
            mFramebuffers[type] = nullptr;
        }
    }

    constexpr GLuint ATTACHMENTS[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

    for (const auto type : { Default, Temp, Ping, Pong })
    {
        mFramebuffers[type] = new QOpenGLFramebufferObject(mWidth, mHeight, mFramebufferFormats[type]);

        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F); // Bloom effect
        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F); // Fragment position
        mFramebuffers[type]->bind();
        glDrawBuffers(3, ATTACHMENTS);
        mFramebuffers[type]->release();
    }
}
