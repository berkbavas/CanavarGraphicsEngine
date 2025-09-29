#include "RenderingManager.h"

#include "Canavar/Engine/Core/RenderingContext.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Manager/ShadowMappingRenderer.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"

void Canavar::Engine::RenderingManager::Initialize()
{
    initializeOpenGLFunctions();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    mFramebufferFormats[Multisample].setSamples(SAMPLES);
    mFramebufferFormats[Multisample].setMipmap(true);
    mFramebufferFormats[Multisample].setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    mFramebufferFormats[Singlesample].setSamples(0);
    mFramebufferFormats[Singlesample].setInternalTextureFormat(QOpenGLTexture::RGBA32F);
    mFramebufferFormats[Aces].setSamples(0);
    mFramebufferFormats[Aces].setInternalTextureFormat(QOpenGLTexture::RGBA32F);
    mFramebufferFormats[Cinematic].setSamples(0);
    mFramebufferFormats[Cinematic].setInternalTextureFormat(QOpenGLTexture::RGBA32F);
    mFramebufferFormats[Temp].setSamples(0);
    mFramebufferFormats[Temp].setInternalTextureFormat(QOpenGLTexture::RGBA32F);

    for (const auto type : FBO_TYPES)
    {
        mFramebuffers[type] = nullptr;
    }

    mQuadData = std::make_shared<QuadData>();
    mSphereData = std::make_shared<SphereData>(1.0f, 72, 36);
    mTorusData = std::make_shared<TorusData>(1.0f, 0.025f, 72, 36);

    ResizeFramebuffers();
}

void Canavar::Engine::RenderingManager::PostInitialize()
{
    const auto pRenderingContext = GetRenderingContext();
    mShaderManager = pRenderingContext->GetShaderManager();
    mNodeManager = pRenderingContext->GetNodeManager();
    mCameraManager = pRenderingContext->GetCameraManager();
    mLightManager = pRenderingContext->GetLightManager();
    mShadowMappingRenderer = pRenderingContext->GetShadowMappingRenderer();

    mSky = mNodeManager->GetSky();
    mSun = mNodeManager->GetSun();
    mHaze = mNodeManager->GetHaze();
    mTerrain = mNodeManager->GetTerrain();

    mModelShader = mShaderManager->GetShader(ShaderType::Model);
    mSkyShader = mShaderManager->GetShader(ShaderType::Sky);
    mCinematicShader = mShaderManager->GetShader(ShaderType::Cinematic);
    mNozzleEffectShader = mShaderManager->GetShader(ShaderType::NozzleEffect);
    mLightningStrikeShader = mShaderManager->GetShader(ShaderType::LightningStrike);
    mLineShader = mShaderManager->GetShader(ShaderType::Line);
    mTerrainShader = mShaderManager->GetShader(ShaderType::Terrain);
    mAcesShader = mShaderManager->GetShader(ShaderType::Aces);
    mScreenShader = mShaderManager->GetShader(ShaderType::Screen);
    mBasicShader = mShaderManager->GetShader(ShaderType::Basic);
}

void Canavar::Engine::RenderingManager::Shutdown()
{
    DestroyFramebuffers();
    mQuadData.reset();
}

void Canavar::Engine::RenderingManager::Update(float ifps)
{
    mIfps = ifps;
    mTime += ifps;
    mDevicePixelRatio = GetRenderingContext()->GetDevicePixelRatio();
}

void Canavar::Engine::RenderingManager::Render(PerspectiveCamera* pActiveCamera)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glDisable(GL_BLEND);

    RenderToFramebuffer(mFramebuffers[Multisample], pActiveCamera);

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

    DoPostProcessing();
}

void Canavar::Engine::RenderingManager::DoPostProcessing()
{
    QOpenGLFramebufferObject::blitFramebuffer(mFramebuffers[Temp], mFramebuffers[Singlesample], GL_COLOR_BUFFER_BIT, GL_NEAREST);

    ApplyAcesPass();
    ApplyCinematicPass();

    QOpenGLFramebufferObject::bindDefault();
    glViewport(0, 0, mWidth * mDevicePixelRatio, mHeight * mDevicePixelRatio);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mScreenShader->Bind();
    mScreenShader->SetSampler("uColorTexture", 0, mFramebuffers[Temp]->textures().at(0));
    mQuadData->Render();
    mScreenShader->Release();
}

void Canavar::Engine::RenderingManager::ApplyAcesPass()
{
    if (!mAcesEnabled)
    {
        return;
    }

    mFramebuffers[Aces]->bind();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mAcesShader->Bind();
    mAcesShader->SetSampler("uSceneTexture", 0, mFramebuffers[Temp]->texture());
    mAcesShader->SetUniformValue("uExposure", mExposure);
    mQuadData->Render();
    mAcesShader->Release();
    mFramebuffers[Aces]->release();

    QOpenGLFramebufferObject::blitFramebuffer(mFramebuffers[Temp], mFramebuffers[Aces], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Canavar::Engine::RenderingManager::ApplyCinematicPass()
{
    if (!mCinematicEnabled)
    {
        return;
    }

    mFramebuffers[Cinematic]->bind();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mCinematicShader->Bind();
    mCinematicShader->SetSampler("uSceneTexture", 0, mFramebuffers[Temp]->texture());
    mCinematicShader->SetUniformValue("uTime", mTime);
    mCinematicShader->SetUniformValue("uGrainStrength", mGrainStrength);
    mCinematicShader->SetUniformValue("uResolution", QVector2D(mWidth, mHeight));
    mCinematicShader->SetUniformValue("uVignetteRadius", mVignetteRadius);
    mCinematicShader->SetUniformValue("uVignetteSoftness", mVignetteSoftness);
    mQuadData->Render();
    mCinematicShader->Release();
    mFramebuffers[Cinematic]->release();

    QOpenGLFramebufferObject::blitFramebuffer(mFramebuffers[Temp], mFramebuffers[Cinematic], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Canavar::Engine::RenderingManager::DestroyFramebuffers()
{
    for (const auto type : FBO_TYPES)
    {
        if (mFramebuffers[type])
        {
            delete mFramebuffers[type];
            mFramebuffers[type] = nullptr;
        }
    }
}

QVector2D Canavar::Engine::RenderingManager::CalculateSunScreenSpacePosition() const
{
    const auto lightPosition = 100000 * mSun->GetDirection();
    const auto lightPosWorld = QVector4D(lightPosition.x(), lightPosition.y(), lightPosition.z(), 1.0f);
    auto sunClipSpaceCoords = mActiveCamera->GetViewProjectionMatrix() * lightPosWorld;
    sunClipSpaceCoords /= sunClipSpaceCoords.w(); // NDC

    return QVector2D(sunClipSpaceCoords.x() * 0.5f + 0.5f, sunClipSpaceCoords.y() * 0.5f + 0.5f);
}

void Canavar::Engine::RenderingManager::RenderToFramebuffer(QOpenGLFramebufferObject* pFramebuffer, PerspectiveCamera* pActiveCamera)
{
    pFramebuffer->bind();

    mActiveCamera = pActiveCamera;

    glViewport(0, 0, pFramebuffer->width(), pFramebuffer->height());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SetCommonUniforms(mModelShader, pActiveCamera);
    SetDirectionalLights(mModelShader);

    SetCommonUniforms(mBasicShader, pActiveCamera);
    SetDirectionalLights(mBasicShader);

    RenderSky(mActiveCamera);
    RenderTerrain(mTerrain.get(), mActiveCamera);
    RenderObjects(mActiveCamera);

    emit RenderLoop();

    pFramebuffer->release();
}

void Canavar::Engine::RenderingManager::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    ResizeFramebuffers();
}

void Canavar::Engine::RenderingManager::RenderObjects(PerspectiveCamera* pCamera)
{
    const auto& Nodes = mNodeManager->GetNodes();

    // Opaque render pass

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
                RenderModel(pModel.get(), RenderPass::Opaque);
            }
            else if (PrimitiveMeshPtr pPrimitiveMesh = std::dynamic_pointer_cast<PrimitiveMesh>(pObject))
            {
                RenderPrimitiveMesh(pPrimitiveMesh.get(), RenderPass::Opaque);
            }
            else if (NozzleEffectPtr pEffect = std::dynamic_pointer_cast<NozzleEffect>(pObject))
            {
                RenderNozzleEffect(pEffect.get(), pCamera);
            }
            else if (LightningStrikeBasePtr pLightning = std::dynamic_pointer_cast<LightningStrikeBase>(pObject))
            {
                pLightning->Render(pCamera, mLightningStrikeShader, mLineShader, mIfps);
            }
        }
    }

    // Transparent render pass

    glDepthMask(GL_FALSE);

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
                RenderModel(pModel.get(), RenderPass::Transparent);
            }
            else if (PrimitiveMeshPtr pPrimitiveMesh = std::dynamic_pointer_cast<PrimitiveMesh>(pObject))
            {
                RenderPrimitiveMesh(pPrimitiveMesh.get(), RenderPass::Transparent);
            }
        }
    }

    glDepthMask(GL_TRUE);
}

void Canavar::Engine::RenderingManager::RenderModel(Model* pModel, RenderPass RenderPass)
{
    SetPointLights(mModelShader, pModel);

    mModelShader->Bind();
    mModelShader->SetUniformValue("uSelectedMeshId", pModel->GetSelectedMeshId());
    mModelShader->SetUniformValue("uModel.Color", pModel->GetColor());
    mModelShader->SetUniformValue("uModel.TransparencyColor", pModel->GetTransparencyColor());
    mModelShader->SetUniformValue("uModel.ShadingMode", pModel->GetShadingMode());
    mModelShader->SetUniformValue("uModel.UseModelColor", pModel->GetUseModelColor());
    mModelShader->SetUniformValue("uModel.Ambient", pModel->GetAmbient());
    mModelShader->SetUniformValue("uModel.Diffuse", pModel->GetDiffuse());
    mModelShader->SetUniformValue("uModel.Specular", pModel->GetSpecular());
    mModelShader->SetUniformValue("uModel.Shininess", pModel->GetShininess());

    if (const auto pScene = mNodeManager->GetScene(pModel))
    {
        pScene->Render(pModel, mModelShader, RenderPass);
    }
    else
    {
        LOG_FATAL("RenderingManager::RenderModel: Model data is not found for this model: {}", pModel->GetSceneName());
    }
}

void Canavar::Engine::RenderingManager::RenderPrimitiveMesh(PrimitiveMesh* pPrimitiveMesh, RenderPass RenderPass)
{
    if (RenderPass == RenderPass::Opaque && pPrimitiveMesh->GetOpacity() < 1.0f)
    {
        return;
    }

    if (RenderPass == RenderPass::Transparent && pPrimitiveMesh->GetOpacity() >= 1.0f)
    {
        return;
    }

    SetPointLights(mBasicShader, pPrimitiveMesh);

    mBasicShader->Bind();
    mBasicShader->SetUniformValue("uModel.Color", pPrimitiveMesh->GetColor());
    mBasicShader->SetUniformValue("uModel.Ambient", pPrimitiveMesh->GetAmbient());
    mBasicShader->SetUniformValue("uModel.Diffuse", pPrimitiveMesh->GetDiffuse());
    mBasicShader->SetUniformValue("uModel.Specular", pPrimitiveMesh->GetSpecular());
    mBasicShader->SetUniformValue("uModel.Shininess", pPrimitiveMesh->GetShininess());
    mBasicShader->SetUniformValue("uModel.Opacity", pPrimitiveMesh->GetOpacity());
    mBasicShader->SetUniformValue("uNodeId", pPrimitiveMesh->GetNodeId());
    mBasicShader->SetUniformValue("uModelMatrix", pPrimitiveMesh->GetTransformation());
    mBasicShader->SetUniformValue("uNormalMatrix", pPrimitiveMesh->GetLocalNormalMatrix());

    if (const auto* pSphere = dynamic_cast<Sphere*>(pPrimitiveMesh))
    {
        mSphereData->Render();
    }
    else if (const auto* pTorus = dynamic_cast<Torus*>(pPrimitiveMesh))
    {
        mTorusData->Render();
    }

    mBasicShader->Release();
}

void Canavar::Engine::RenderingManager::RenderNozzleEffect(NozzleEffect* pEffect, PerspectiveCamera* pCamera)
{
    mNozzleEffectShader->Bind();
    mNozzleEffectShader->SetUniformValue("uMaxRadius", pEffect->GetMaxRadius());
    mNozzleEffectShader->SetUniformValue("uMVP", pCamera->GetViewProjectionMatrix() * pEffect->GetWorldTransformation());
    mNozzleEffectShader->SetUniformValue("uFarPlane", pCamera->GetZFar());
    pEffect->Render(mIfps);
    mNozzleEffectShader->Release();
}

void Canavar::Engine::RenderingManager::RenderSky(PerspectiveCamera* pCamera)
{
    mSky->Render(mSkyShader, mSun.get(), pCamera);
}

void Canavar::Engine::RenderingManager::RenderTerrain(Terrain* pTerrain, PerspectiveCamera* pCamera)
{
    const auto& DirectonalLights = mLightManager->GetDirectionalLights();
    const auto NumberOfDirectonalLights = std::min(8, static_cast<int>(DirectonalLights.size()));

    const auto& PointLights = mLightManager->GetPointLightsAround(pCamera->GetWorldPosition(), 20'000.0f);
    const auto NumberOfPointLights = std::min(8, static_cast<int>(PointLights.size()));

    mTerrainShader->Bind();

    mTerrainShader->SetUniformValue("uViewProjectionMatrix", pCamera->GetViewProjectionMatrix());
    mTerrainShader->SetUniformValue("uLightViewProjectionMatrix", mShadowMappingRenderer->GetLightViewProjectionMatrix());
    mTerrainShader->SetUniformValue("uFarPlane", pCamera->GetZFar());
    mTerrainShader->SetUniformValue("uCameraPosition", pCamera->GetWorldPosition());
    mTerrainShader->SetUniformValue("uHaze.Enabled", mHaze->GetEnabled());
    mTerrainShader->SetUniformValue("uHaze.Color", mHaze->GetColor());
    mTerrainShader->SetUniformValue("uHaze.Density", mHaze->GetDensity());
    mTerrainShader->SetUniformValue("uHaze.Gradient", mHaze->GetGradient());
    mTerrainShader->SetUniformValue("uNumberOfDirectionalLights", NumberOfDirectonalLights);
    mTerrainShader->SetUniformValue("uNumberOfPointLights", NumberOfPointLights);
    mTerrainShader->SetSampler("uShadow.Map", SHADOW_MAP_TEXTURE_UNIT, mShadowMappingRenderer->GetShadowMapTexture());
    mTerrainShader->SetUniformValue("uShadow.Enabled", mShadowMappingRenderer->GetShadowsEnabled());
    mTerrainShader->SetUniformValue("uShadow.Bias", mShadowMappingRenderer->GetShadowBias());
    mTerrainShader->SetUniformValue("uShadow.Samples", mShadowMappingRenderer->GetShadowSamples());

    for (int i = 0; i < NumberOfDirectonalLights; i++)
    {
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Direction", DirectonalLights[i]->GetDirection());
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Color", DirectonalLights[i]->GetColor());
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Ambient", DirectonalLights[i]->GetAmbient());
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Diffuse", DirectonalLights[i]->GetDiffuse());
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Specular", DirectonalLights[i]->GetSpecular());
    }

    for (int i = 0; i < NumberOfPointLights; i++)
    {
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Color", PointLights[i]->GetColor());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Position", PointLights[i]->GetWorldPosition());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Ambient", PointLights[i]->GetAmbient());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Diffuse", PointLights[i]->GetDiffuse());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Specular", PointLights[i]->GetSpecular());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Constant", PointLights[i]->GetConstant());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Linear", PointLights[i]->GetLinear());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Quadratic", PointLights[i]->GetQuadratic());
    }

    mTerrainShader->Release();
    mTerrain->Render(mTerrainShader, pCamera);
}

void Canavar::Engine::RenderingManager::SetCommonUniforms(Shader* pShader, PerspectiveCamera* pCamera)
{
    pShader->Bind();
    pShader->SetUniformValue("uFarPlane", pCamera->GetZFar());

    if (pShader->GetShaderType() == ShaderType::Model)
    {
        pShader->SetUniformValue("uMeshSelectionEnabled", static_cast<int>(mMeshSelectionEnabled));
    }

    if (pShader->GetShaderType() == ShaderType::Basic || pShader->GetShaderType() == ShaderType::Model)
    {
        pShader->SetUniformValue("uLightViewProjectionMatrix", mShadowMappingRenderer->GetLightViewProjectionMatrix());
        pShader->SetSampler("uShadow.Map", SHADOW_MAP_TEXTURE_UNIT, mShadowMappingRenderer->GetShadowMapTexture());
        pShader->SetUniformValue("uShadow.Enabled", mShadowMappingRenderer->GetShadowsEnabled());
        pShader->SetUniformValue("uShadow.Bias", mShadowMappingRenderer->GetShadowBias());
        pShader->SetUniformValue("uShadow.Samples", mShadowMappingRenderer->GetShadowSamples());
    }

    pShader->SetUniformValue("uHaze.Enabled", mHaze->GetEnabled());
    pShader->SetUniformValue("uHaze.Color", mHaze->GetColor());
    pShader->SetUniformValue("uHaze.Density", mHaze->GetDensity());
    pShader->SetUniformValue("uHaze.Gradient", mHaze->GetGradient());
    pShader->SetUniformValue("uCameraPosition", pCamera->GetWorldPosition());
    pShader->SetUniformValue("uViewProjectionMatrix", pCamera->GetViewProjectionMatrix());
    pShader->Release();
}

void Canavar::Engine::RenderingManager::SetDirectionalLights(Shader* pShader)
{
    const auto& Lights = mLightManager->GetDirectionalLights();
    const auto NumberOfLights = std::min(8, static_cast<int>(Lights.size()));

    pShader->Bind();
    pShader->SetUniformValue("uNumberOfDirectionalLights", NumberOfLights);

    for (int i = 0; i < NumberOfLights; i++)
    {
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Direction", Lights[i]->GetDirection());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Color", Lights[i]->GetColor());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Ambient", Lights[i]->GetAmbient());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Diffuse", Lights[i]->GetDiffuse());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Specular", Lights[i]->GetSpecular());

        if (pShader == mModelShader)
        {
            pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].Radiance", Lights[i]->GetRadiance());
        }
    }

    pShader->Release();
}

void Canavar::Engine::RenderingManager::SetPointLights(Shader* pShader, Object* pObject)
{
    const auto& Lights = mLightManager->GetPointLightsAround(pObject->GetWorldPosition(), 100'000.0f);
    const auto NumberOfLights = std::min(8, static_cast<int>(Lights.size()));

    pShader->Bind();
    pShader->SetUniformValue("uNumberOfPointLights", NumberOfLights);

    for (int i = 0; i < NumberOfLights; i++)
    {
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Color", Lights[i]->GetColor());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Position", Lights[i]->GetWorldPosition());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Ambient", Lights[i]->GetAmbient());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Diffuse", Lights[i]->GetDiffuse());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Specular", Lights[i]->GetSpecular());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Constant", Lights[i]->GetConstant());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Linear", Lights[i]->GetLinear());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].Quadratic", Lights[i]->GetQuadratic());
    }

    pShader->Release();
}

void Canavar::Engine::RenderingManager::ResizeFramebuffers()
{
    DestroyFramebuffers();

    for (const auto type : { Multisample, Singlesample })
    {
        mFramebuffers[type] = new QOpenGLFramebufferObject(mWidth, mHeight, mFramebufferFormats[type]);

        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F);
        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F);
        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F);
        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F);

        mFramebuffers[type]->bind();
        glDrawBuffers(NUMBER_OF_FBO_ATTACHMENTS, FBO_ATTACHMENTS);
        mFramebuffers[type]->release();
    }

    for (const auto type : { Aces, Cinematic, Temp })
    {
        mFramebuffers[type] = new QOpenGLFramebufferObject(mWidth, mHeight, mFramebufferFormats[type]);
    }
}

QVector3D Canavar::Engine::RenderingManager::FetchFragmentLocalPositionFromScreen(int x, int y)
{
    QVector3D position;
    mFramebuffers[Singlesample]->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(x, mFramebuffers[Singlesample]->height() - y, 1, 1, GL_RGBA, GL_FLOAT, &position);
    mFramebuffers[Singlesample]->release();
    return position;
}

QVector3D Canavar::Engine::RenderingManager::FetchFragmentWorldPositionFromScreen(int x, int y)
{
    QVector3D position;
    mFramebuffers[Singlesample]->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glReadPixels(x, mFramebuffers[Singlesample]->height() - y, 1, 1, GL_RGBA, GL_FLOAT, &position);
    mFramebuffers[Singlesample]->release();
    return position;
}

Canavar::Engine::NodeInfo Canavar::Engine::RenderingManager::FetchNodeInfoFromScreenCoordinates(int x, int y)
{
    NodeInfo info;
    mFramebuffers[Singlesample]->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT3);
    glReadPixels(x, mFramebuffers[Singlesample]->height() - y, 1, 1, GL_RGBA, GL_FLOAT, &info);
    mFramebuffers[Singlesample]->release();
    return info;
}
