#include "RenderingManager.h"

#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/BoundingBoxRenderer.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"

Canavar::Engine::RenderingManager::RenderingManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::RenderingManager::Initialize()
{
    initializeOpenGLFunctions();

    mFramebufferFormats[Multisample].setSamples(4);
    mFramebufferFormats[Multisample].setMipmap(true);
    mFramebufferFormats[Multisample].setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    mFramebufferFormats[Singlesample].setSamples(0);
    mFramebufferFormats[Temp1].setSamples(0);
    mFramebufferFormats[Temp2].setSamples(0);

    for (const auto type : { Multisample, Singlesample, Temp1, Temp2 })
    {
        mFramebuffers[type] = nullptr;
    }

    mQuad = new Quad;

    mBoundingBoxRenderer = new BoundingBoxRenderer;
    mShadowMappingRenderer = new ShadowMappingRenderer;
    mCrossSectionAnalyzer = new CrossSectionAnalyzer;

    ResizeFramebuffers();
}

void Canavar::Engine::RenderingManager::PostInitialize()
{
    mShaderManager = mManagerProvider->GetShaderManager();
    mNodeManager = mManagerProvider->GetNodeManager();
    mCameraManager = mManagerProvider->GetCameraManager();
    mLightManager = mManagerProvider->GetLightManager();

    mBoundingBoxRenderer->SetShaderManager(mShaderManager);
    mBoundingBoxRenderer->SetNodeManager(mNodeManager);
    mBoundingBoxRenderer->Initialize();

    mSky = mNodeManager->GetSky();
    mSun = mNodeManager->GetSun();
    mTerrain = mNodeManager->GetTerrain();
    mHaze = mNodeManager->GetHaze();

    mShadowMappingRenderer->SetShaderManager(mShaderManager);
    mShadowMappingRenderer->SetNodeManager(mNodeManager);
    mShadowMappingRenderer->SetCameraManager(mCameraManager);
    mShadowMappingRenderer->SetSun(mSun);
    mShadowMappingRenderer->Initialize();

    mCrossSectionAnalyzer->Initialize(mManagerProvider);

    mModelShader = mShaderManager->GetShader(ShaderType::Model);
    mSkyShader = mShaderManager->GetShader(ShaderType::Sky);
    mTerrainShader = mShaderManager->GetShader(ShaderType::Terrain);
    mBlurShader = mShaderManager->GetShader(ShaderType::Blur);
    mPostProcessShader = mShaderManager->GetShader(ShaderType::PostProcess);
    mNozzleEffectShader = mShaderManager->GetShader(ShaderType::NozzleEffect);
    mLightningStrikeShader = mShaderManager->GetShader(ShaderType::LightningStrike);
    mLineShader = mShaderManager->GetShader(ShaderType::Line);
}

void Canavar::Engine::RenderingManager::Render(float ifps)
{
    mIfps = ifps;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    if (mShadowsEnabled)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        mShadowMappingRenderer->Render(ifps);
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
    }

    mActiveCamera = mCameraManager->GetActiveCamera().get();

    mFramebuffers[Multisample]->bind();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SetUniforms(mActiveCamera);

    mSky->Render(mSkyShader, mSun.get(), mActiveCamera);
    mTerrain->Render(mTerrainShader, mActiveCamera);

    RenderObjects(mActiveCamera, ifps);

    if (mDrawBoundingBoxes)
    {
        mBoundingBoxRenderer->Render(mActiveCamera, ifps);
    }

    if (mCrossSectionEnabled)
    {
        mCrossSectionAnalyzer->RenderPlane();
    }

    emit RenderLoop(ifps);

    mFramebuffers[Multisample]->release();

    // ----------------------- BLIT 4X MULTISAMPLE FBO TO SINGLESAMPLE FBO -------------------

    for (const auto attachment : FBO_ATTACHMENTS)
    {
        QOpenGLFramebufferObject::blitFramebuffer( //
            mFramebuffers[Singlesample],
            QRect(0, 0, mFramebuffers[Singlesample]->width(), mFramebuffers[Singlesample]->height()),
            mFramebuffers[Multisample],
            QRect(0, 0, mFramebuffers[Multisample]->width(), mFramebuffers[Multisample]->height()),
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST,
            attachment - GL_COLOR_ATTACHMENT0,
            attachment - GL_COLOR_ATTACHMENT0);
    }

    // ----------------------- BLIT TO DEFAULT FBO -------------------

    QOpenGLFramebufferObject::blitFramebuffer(mFramebuffers[Temp1], mFramebuffers[Multisample], GL_COLOR_BUFFER_BIT, GL_LINEAR);

    mFramebuffers[Temp2]->bind();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mPostProcessShader->Bind();
    mPostProcessShader->SetUniformValue("blurThreshold", mBlurThreshold);
    mPostProcessShader->SetUniformValue("maxSamples", mMaxSamples);
    mPostProcessShader->SetSampler("colorTexture", 0, mFramebuffers[Temp1]->texture());
    mPostProcessShader->SetSampler("distanceTexture", 1, mFramebuffers[Singlesample]->textures().at(4));
    mQuad->Render();
    mPostProcessShader->Release();
    mFramebuffers[Temp2]->release();

    QOpenGLFramebufferObject::blitFramebuffer(nullptr, mFramebuffers[Temp2], GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // For QPainter
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
}

void Canavar::Engine::RenderingManager::RenderToFramebuffer(QOpenGLFramebufferObject* pFramebuffer, Camera* pCamera)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    pFramebuffer->bind();

    glViewport(0, 0, pFramebuffer->width(), pFramebuffer->height());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SetUniforms(pCamera);

    mSky->Render(mSkyShader, mSun.get(), pCamera);
    mTerrain->Render(mTerrainShader, pCamera);

    RenderObjects(pCamera, mIfps);

    // For QPainter
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
}

void Canavar::Engine::RenderingManager::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    ResizeFramebuffers();
}

void Canavar::Engine::RenderingManager::RenderObjects(Camera* pCamera, float ifps)
{
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
                RenderModel(pModel);
            }
            else if (NozzleEffectPtr pEffect = std::dynamic_pointer_cast<NozzleEffect>(pObject))
            {
                RenderNozzleEffect(pEffect, pCamera, ifps);
            }
            else if (LightningStrikeBasePtr pLightning = std::dynamic_pointer_cast<LightningStrikeBase>(pObject))
            {
                pLightning->Render(pCamera, mLightningStrikeShader, mLineShader, ifps);
            }
        }
    }
}

void Canavar::Engine::RenderingManager::RenderModel(ModelPtr pModel)
{
    SetPointLights(mModelShader, pModel.get());

    mModelShader->Bind();
    mModelShader->SetUniformValue("model.useColor", pModel->GetUseColor());
    mModelShader->SetUniformValue("model.color", pModel->GetColor());
    mModelShader->SetUniformValue("model.ambient", pModel->GetAmbient());
    mModelShader->SetUniformValue("model.diffuse", pModel->GetDiffuse());
    mModelShader->SetUniformValue("model.specular", pModel->GetSpecular());
    mModelShader->SetUniformValue("model.shininess", pModel->GetShininess());
    mModelShader->SetUniformValue("model.invertNormals", pModel->GetInvertNormals());

    if (const auto pScene = mNodeManager->GetScene(pModel))
    {
        pScene->Render(pModel.get(), mModelShader);
    }
    else
    {
        LOG_FATAL("RenderingManager::RenderModel: Model data is not found for this model: {}", pModel->GetSceneName().toStdString());
    }

    mModelShader->Release();
}

void Canavar::Engine::RenderingManager::RenderNozzleEffect(NozzleEffectPtr pEffect, Camera* pCamera, float ifps)
{
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_ONE, GL_ONE);
    mNozzleEffectShader->Bind();
    mNozzleEffectShader->SetUniformValue("maxRadius", pEffect->GetMaxRadius());
    mNozzleEffectShader->SetUniformValue("MVP", pCamera->GetViewProjectionMatrix() * pEffect->GetWorldTransformation());
    mNozzleEffectShader->SetUniformValue("zFar", dynamic_cast<PerspectiveCamera*>(pCamera)->GetZFar());
    pEffect->Render(ifps);
    mNozzleEffectShader->Release();
    // glDisable(GL_BLEND);
}

void Canavar::Engine::RenderingManager::SetUniforms(Camera* pCamera)
{
    SetCommonUniforms(mModelShader, pCamera);
    SetCommonUniforms(mTerrainShader, pCamera);
    SetDirectionalLights(mModelShader);
    SetDirectionalLights(mTerrainShader);
    SetPointLights(mTerrainShader, pCamera);

    mModelShader->Bind();
    mModelShader->SetUniformValue("zFar", dynamic_cast<PerspectiveCamera*>(pCamera)->GetZFar());
    mModelShader->Release();
}

void Canavar::Engine::RenderingManager::SetCommonUniforms(Shader* pShader, Camera* pCamera)
{
    pShader->Bind();
    pShader->SetUniformValue("haze.enabled", mHaze->GetEnabled());
    pShader->SetUniformValue("haze.color", mHaze->GetColor());
    pShader->SetUniformValue("haze.density", mHaze->GetDensity());
    pShader->SetUniformValue("haze.gradient", mHaze->GetGradient());
    pShader->SetUniformValue("cameraPosition", pCamera->GetWorldPosition());
    pShader->SetUniformValue("VP", pCamera->GetViewProjectionMatrix());
    pShader->SetUniformValue("LVP", mShadowMappingRenderer->GetLightViewProjectionMatrix());
    pShader->SetSampler("shadow.map", SHADOW_MAP_TEXTURE_UNIT, mShadowMappingRenderer->GetShadowMap());
    pShader->SetUniformValue("shadow.enabled", mShadowsEnabled);
    pShader->SetUniformValue("shadow.bias", mShadowBias);
    pShader->SetUniformValue("shadow.samples", mShadowSamples);
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

void Canavar::Engine::RenderingManager::SetPointLights(Shader* pShader, Object* pObject)
{
    const auto& lights = mLightManager->GetPointLightsAround(pObject->GetWorldPosition(), 100'000.0f);
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
    for (const auto type : { Multisample, Singlesample, Temp1, Temp2 })
    {
        if (mFramebuffers[type])
        {
            delete mFramebuffers[type];
            mFramebuffers[type] = nullptr;
        }
    }

    for (const auto type : { Multisample, Singlesample })
    {
        mFramebuffers[type] = new QOpenGLFramebufferObject(mWidth, mHeight, mFramebufferFormats[type]);

        for (int i = 1; i < NUMBER_OF_FBO_ATTACHMENTS; ++i)
        {
            mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F);
        }

        mFramebuffers[type]->bind();
        glDrawBuffers(NUMBER_OF_FBO_ATTACHMENTS, FBO_ATTACHMENTS);
        mFramebuffers[type]->release();
    }

    for (const auto type : { Temp1, Temp2 })
    {
        mFramebuffers[type] = new QOpenGLFramebufferObject(mWidth, mHeight, mFramebufferFormats[type]);
    }
}

// void Canavar::Engine::RenderingManager::ApplyPingPong(Framebuffer target, int source, int pass)
// {
//     // Default Framebuffer -> Ping Framebuffer

//     QOpenGLFramebufferObject::blitFramebuffer( //
//         mFramebuffers[Ping],
//         QRect(0, 0, mWidth, mHeight),
//         mFramebuffers[Default],
//         QRect(0, 0, mWidth, mHeight),
//         GL_COLOR_BUFFER_BIT,
//         GL_NEAREST,
//         source, // <- "source" location of Default FBO
//         0       // <- Write location of Ping FBO
//     );

//     // Ping -> Pong -> Ping -> ... Pxng
//     for (int i = 0; i < qMax(0, pass); i++)
//     {
//         mFramebuffers[i % 2 == 0 ? Pong : Ping]->bind();
//         mBlurShader->Bind();
//         mBlurShader->SetUniformValue("horizontal", i % 2 == 0);
//         mBlurShader->SetSampler("targetTexture", 0, mFramebuffers[i % 2 == 0 ? Ping : Pong]->texture());
//         mQuad->Render();
//         mBlurShader->Release();
//     }

//     QOpenGLFramebufferObject::blitFramebuffer( //
//         mFramebuffers[target],
//         QRect(0, 0, mWidth, mHeight),
//         mFramebuffers[qMax(0, pass) % 2 == 0 ? Ping : Pong],
//         QRect(0, 0, mWidth, mHeight),
//         GL_COLOR_BUFFER_BIT,
//         GL_NEAREST,
//         0,
//         0);
// }

QVector3D Canavar::Engine::RenderingManager::FetchFragmentLocalPositionFromScreen(int x, int y)
{
    QVector3D position;
    mFramebuffers[Singlesample]->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(mDevicePixelRatio * x, mFramebuffers[Singlesample]->height() - mDevicePixelRatio * y, 1, 1, GL_RGBA, GL_FLOAT, &position);
    mFramebuffers[Singlesample]->release();
    return position;
}

QVector3D Canavar::Engine::RenderingManager::FetchFragmentWorldPositionFromScreen(int x, int y)
{
    QVector3D position;
    mFramebuffers[Singlesample]->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glReadPixels(mDevicePixelRatio * x, mFramebuffers[Singlesample]->height() - mDevicePixelRatio * y, 1, 1, GL_RGBA, GL_FLOAT, &position);
    mFramebuffers[Singlesample]->release();
    return position;
}

Canavar::Engine::NodeInfo Canavar::Engine::RenderingManager::FetchNodeInfoFromScreenCoordinates(int x, int y)
{
    NodeInfo info;
    mFramebuffers[Singlesample]->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT3);
    glReadPixels(mDevicePixelRatio * x, mFramebuffers[Singlesample]->height() - mDevicePixelRatio * y, 1, 1, GL_RGBA, GL_FLOAT, &info);
    mFramebuffers[Singlesample]->release();
    return info;
}

QOpenGLFramebufferObject* Canavar::Engine::RenderingManager::GetFramebuffer(Framebuffer framebufferType) const
{
    const auto it = mFramebuffers.find(framebufferType);

    if (it != mFramebuffers.end())
    {
        return it->second;
    }

    return nullptr;
}
