#include "RenderingManager.h"

#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/BoundingBoxRenderer.h"
#include "Canavar/Engine/Manager/RenderingManager/TextRenderer.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"

Canavar::Engine::RenderingManager::RenderingManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::RenderingManager::Initialize()
{
    initializeOpenGLFunctions();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    mFramebufferFormats[Multisample].setSamples(SAMPLES);
    mFramebufferFormats[Multisample].setMipmap(true);
    mFramebufferFormats[Multisample].setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    mFramebufferFormats[Singlesample].setSamples(0);
    mFramebufferFormats[Singlesample].setInternalTextureFormat(QOpenGLTexture::RGBA32F);
    mFramebufferFormats[MotionBlur].setSamples(0);
    mFramebufferFormats[MotionBlur].setInternalTextureFormat(QOpenGLTexture::RGBA32F);
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

    mQuad = new Quad;

    mBoundingBoxRenderer = new BoundingBoxRenderer;
    mTextRenderer = new TextRenderer;
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

    mTextRenderer->SetShaderManager(mShaderManager);
    mTextRenderer->SetNodeManager(mNodeManager);
    mTextRenderer->Initialize();

    mSky = mNodeManager->GetSky();
    mSun = mNodeManager->GetSun();
    mHaze = mNodeManager->GetHaze();
    mTerrain = mNodeManager->GetTerrain();

    mShadowMappingRenderer->SetShaderManager(mShaderManager);
    mShadowMappingRenderer->SetNodeManager(mNodeManager);
    mShadowMappingRenderer->SetCameraManager(mCameraManager);
    mShadowMappingRenderer->SetSun(mSun);
    mShadowMappingRenderer->Initialize();

    mCrossSectionAnalyzer->Initialize(mManagerProvider);

    mModelShader = mShaderManager->GetShader(ShaderType::Model);
    mSkyShader = mShaderManager->GetShader(ShaderType::Sky);
    mCinematicShader = mShaderManager->GetShader(ShaderType::Cinematic);
    mNozzleEffectShader = mShaderManager->GetShader(ShaderType::NozzleEffect);
    mLightningStrikeShader = mShaderManager->GetShader(ShaderType::LightningStrike);
    mLineShader = mShaderManager->GetShader(ShaderType::Line);
    mTerrainShader = mShaderManager->GetShader(ShaderType::Terrain);
    mBrightPassShader = mShaderManager->GetShader(ShaderType::BrightPass);
    mGodRaysShader = mShaderManager->GetShader(ShaderType::GodRays);
    mCompositionShader = mShaderManager->GetShader(ShaderType::Composition);
    mAcesShader = mShaderManager->GetShader(ShaderType::Aces);
    mMotionBlurShader = mShaderManager->GetShader(ShaderType::MotionBlur);
}

void Canavar::Engine::RenderingManager::Render(float ifps)
{
    mIfps = ifps;
    mTime += ifps;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glDisable(GL_BLEND);

    mShadowsEnabled ? mShadowMappingRenderer->Render(ifps) : void(0);

    mActiveCamera = dynamic_cast<PerspectiveCamera *>(mCameraManager->GetActiveCamera().get());

    RenderToFramebuffer(mFramebuffers[Multisample], mActiveCamera);

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

    mPreviousViewProjectionMatrix = mActiveCamera->GetViewProjectionMatrix();
}

void Canavar::Engine::RenderingManager::DoPostProcessing()
{
    // Bright Pass
    // {
    //     mFramebuffers[Pong]->bind();
    //     glViewport(0, 0, mWidth, mHeight);
    //     glClearColor(0, 0, 0, 0);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //     mBrightPassShader->Bind();
    //     mBrightPassShader->SetUniformValue("uThreshold", mBrightnessThreshold);
    //     mBrightPassShader->SetSampler("uSceneTexture", 0, mFramebuffers[Singlesample]->textures().at(0));
    //     mQuad->Render();
    //     mBrightPassShader->Release();
    //     mFramebuffers[Pong]->release();
    // }

    // // God Rays
    // {
    //     mFramebuffers[Ping]->bind();
    //     glViewport(0, 0, mWidth, mHeight);
    //     glClearColor(0, 0, 0, 0);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //     mGodRaysShader->Bind();
    //     mGodRaysShader->SetSampler("uBrightTexture", 0, mFramebuffers[Pong]->texture());
    //     mGodRaysShader->SetSampler("uDepthTexture", 1, mFramebuffers[Singlesample]->textures().at(4));
    //     mGodRaysShader->SetUniformValue("uScreenSpaceLightPosition", CalculateSunScreenSpacePosition());
    //     mGodRaysShader->SetUniformValue("uInverseProjection", mActiveCamera->GetProjectionMatrix().inverted());
    //     mGodRaysShader->SetUniformValue("uNumSamples", mNumberOfSamples);
    //     mGodRaysShader->SetUniformValue("uDensity", mDensity);
    //     mGodRaysShader->SetUniformValue("uDecay", mDecay);
    //     mGodRaysShader->SetUniformValue("uWeight", mWeight);
    //     mGodRaysShader->SetUniformValue("uExposure", mExposure);
    //     mQuad->Render();
    //     mGodRaysShader->Release();
    //     mFramebuffers[Ping]->release();
    // }

    // // Composition Pass
    // {
    //     mFramebuffers[Pong]->bind();
    //     glViewport(0, 0, mWidth, mHeight);
    //     glClearColor(0, 0, 0, 0);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //     mCompositionShader->Bind();
    //     mCompositionShader->SetSampler("uSceneTexture", 0, mFramebuffers[Singlesample]->textures().at(0));
    //     mCompositionShader->SetSampler("uRaysTexture", 1, mFramebuffers[Ping]->texture());
    //     mQuad->Render();
    //     mCompositionShader->Release();
    //     mFramebuffers[Pong]->release();
    // }

    QOpenGLFramebufferObject::blitFramebuffer(mFramebuffers[Temp], mFramebuffers[Singlesample], GL_COLOR_BUFFER_BIT, GL_NEAREST);

    if (mMotionBlurEnabled)
    {
        ApplyMotionBlurPass();
    }

    if (mAcesEnabled)
    {
        ApplyAcesPass();
    }

    if (mCinematicEnabled)
    {
        ApplyCinematicPass();
    }

    QOpenGLFramebufferObject::blitFramebuffer(nullptr, mFramebuffers[Temp], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Canavar::Engine::RenderingManager::ApplyAcesPass()
{
    mFramebuffers[Aces]->bind();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mAcesShader->Bind();
    mAcesShader->SetSampler("uSceneTexture", 0, mFramebuffers[Temp]->texture());
    mAcesShader->SetUniformValue("uExposure", mExposure);
    mQuad->Render();
    mAcesShader->Release();
    mFramebuffers[Aces]->release();

    QOpenGLFramebufferObject::blitFramebuffer(mFramebuffers[Temp], mFramebuffers[Aces], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Canavar::Engine::RenderingManager::ApplyCinematicPass()
{
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
    mQuad->Render();
    mCinematicShader->Release();
    mFramebuffers[Cinematic]->release();

    QOpenGLFramebufferObject::blitFramebuffer(mFramebuffers[Temp], mFramebuffers[Cinematic], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Canavar::Engine::RenderingManager::ApplyMotionBlurPass()
{
    mFramebuffers[MotionBlur]->bind();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mMotionBlurShader->Bind();
    mMotionBlurShader->SetSampler("uSceneTexture", 9, mFramebuffers[Temp]->texture());
    mMotionBlurShader->SetSampler("uVelocityTexture", 10, mFramebuffers[Singlesample]->textures().at(4));
    mMotionBlurShader->SetUniformValue("uStrength", mMotionBlurStrength);
    mMotionBlurShader->SetUniformValue("uNumberOfSamples", mMotionBlurSamples);
    mMotionBlurShader->SetUniformValue("uZNear", mActiveCamera->GetZNear());
    mMotionBlurShader->SetUniformValue("uZFar", mActiveCamera->GetZFar());
    mMotionBlurShader->SetUniformValue("uDepthThreshold", mMotionBlurDepthThreshold);
    mQuad->Render();
    mMotionBlurShader->Release();
    mFramebuffers[MotionBlur]->release();

    QOpenGLFramebufferObject::blitFramebuffer(mFramebuffers[Temp], mFramebuffers[MotionBlur], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

QVector2D Canavar::Engine::RenderingManager::CalculateSunScreenSpacePosition() const
{
    const auto lightPosition = 100000 * mSun->GetDirection();
    const auto lightPosWorld = QVector4D(lightPosition.x(), lightPosition.y(), lightPosition.z(), 1.0f);
    auto sunClipSpaceCoords = mActiveCamera->GetViewProjectionMatrix() * lightPosWorld;
    sunClipSpaceCoords /= sunClipSpaceCoords.w(); // NDC

    return QVector2D(sunClipSpaceCoords.x() * 0.5f + 0.5f, sunClipSpaceCoords.y() * 0.5f + 0.5f);
}

void Canavar::Engine::RenderingManager::RenderToFramebuffer(QOpenGLFramebufferObject* pFramebuffer, Camera* pCamera)
{
    pFramebuffer->bind();

    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SetUniforms(mActiveCamera);

    RenderSky(mActiveCamera);
    RenderTerrain(mTerrain.get(), mActiveCamera);
    RenderObjects(mActiveCamera, mIfps);

    mDrawBoundingBoxes ? mBoundingBoxRenderer->Render(mActiveCamera, mIfps) : void(0);
    mCrossSectionEnabled ? mCrossSectionAnalyzer->RenderPlane() : void(0);
    mTextRenderer->Render(mActiveCamera);

    emit RenderLoop(mIfps);

    pFramebuffer->release();
}

void Canavar::Engine::RenderingManager::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    mTextRenderer->Resize(width, height);

    ResizeFramebuffers();
}

void Canavar::Engine::RenderingManager::RenderObjects(Camera* pCamera, float ifps)
{
    const auto& Nodes = mNodeManager->GetNodes();

    // Opaque render pass
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QVector<Model*> VisibleModels;

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
                VisibleModels.append(pModel.get());
                RenderModel(pModel.get(), RenderPass::Opaque);
            }
            else if (NozzleEffectPtr pEffect = std::dynamic_pointer_cast<NozzleEffect>(pObject))
            {
                RenderNozzleEffect(pEffect.get(), pCamera, ifps);
            }
            else if (LightningStrikeBasePtr pLightning = std::dynamic_pointer_cast<LightningStrikeBase>(pObject))
            {
                pLightning->Render(pCamera, mLightningStrikeShader, mLineShader, ifps);
            }
        }
    }

    // Transparent render pass

    // First add transparent meshes to render list
    QVector<TransparentMeshListElement> TransparentMeshes;

    for (auto* pModel : VisibleModels)
    {
        if (const auto pScene = mNodeManager->GetScene(pModel))
        {
            pScene->AddMeshesToListIfHasTransparency(pModel, TransparentMeshes);
        }
    }

    // Sort transparent meshes back to front
    std::sort(TransparentMeshes.begin(), TransparentMeshes.end(), [&](const TransparentMeshListElement& left, const TransparentMeshListElement& right) {
        const auto l = left.CombinedTransformation * ORIGIN;
        const auto r = right.CombinedTransformation * ORIGIN;

        float dl = (pCamera->GetWorldPosition() - l).length();
        float dr = (pCamera->GetWorldPosition() - r).length();

        return dl > dr;
    });

    // Render transparent objects
    glDepthMask(GL_FALSE);

    for (const auto& Element : TransparentMeshes)
    {
        const auto pModel = Element.pModel;

        mModelShader->Bind();
        mModelShader->SetUniformValue("uModel.color", pModel->GetColor());
        mModelShader->SetUniformValue("uModel.transparencyColor", pModel->GetTransparencyColor());
        mModelShader->SetUniformValue("uModel.shadingMode", pModel->GetShadingMode());
        mModelShader->SetUniformValue("uModel.useModelColor", pModel->GetUseModelColor());
        mModelShader->SetUniformValue("uModel.ambient", pModel->GetAmbient());
        mModelShader->SetUniformValue("uModel.diffuse", pModel->GetDiffuse());
        mModelShader->SetUniformValue("uModel.specular", pModel->GetSpecular());
        mModelShader->SetUniformValue("uModel.shininess", pModel->GetShininess());
        mModelShader->SetUniformValue("uLightViewProjectionMatrix", mShadowMappingRenderer->GetLightViewProjectionMatrix());
        mModelShader->SetSampler("uShadow.map", SHADOW_MAP_TEXTURE_UNIT, mShadowMappingRenderer->GetShadowMapTexture());
        mModelShader->SetUniformValue("uShadow.enabled", mShadowsEnabled);
        mModelShader->SetUniformValue("uShadow.bias", mShadowBias);
        mModelShader->SetUniformValue("uShadow.samples", mShadowSamples);
        Element.pMesh->Render(pModel, mModelShader, Element.NodeMatrix, RenderPass::Transparent);
        mModelShader->Release();
    }

    glDepthMask(GL_TRUE);
}

void Canavar::Engine::RenderingManager::RenderModel(Model* pModel, RenderPass RenderPass)
{
    SetPointLights(mModelShader, pModel);

    mModelShader->Bind();
    mModelShader->SetUniformValue("uSelectedMeshId", pModel->GetSelectedMeshId());
    mModelShader->SetUniformValue("uModel.color", pModel->GetColor());
    mModelShader->SetUniformValue("uModel.transparencyColor", pModel->GetTransparencyColor());
    mModelShader->SetUniformValue("uModel.shadingMode", pModel->GetShadingMode());
    mModelShader->SetUniformValue("uModel.useModelColor", pModel->GetUseModelColor());
    mModelShader->SetUniformValue("uModel.ambient", pModel->GetAmbient());
    mModelShader->SetUniformValue("uModel.diffuse", pModel->GetDiffuse());
    mModelShader->SetUniformValue("uModel.specular", pModel->GetSpecular());
    mModelShader->SetUniformValue("uModel.shininess", pModel->GetShininess());
    mModelShader->SetUniformValue("uLightViewProjectionMatrix", mShadowMappingRenderer->GetLightViewProjectionMatrix());
    mModelShader->SetSampler("uShadow.map", SHADOW_MAP_TEXTURE_UNIT, mShadowMappingRenderer->GetShadowMapTexture());
    mModelShader->SetUniformValue("uShadow.enabled", mShadowsEnabled);
    mModelShader->SetUniformValue("uShadow.bias", mShadowBias);
    mModelShader->SetUniformValue("uShadow.samples", mShadowSamples);

    if (const auto pScene = mNodeManager->GetScene(pModel))
    {
        pScene->Render(pModel, mModelShader, RenderPass);
    }
    else
    {
        LOG_FATAL("RenderingManager::RenderModel: Model data is not found for this model: {}", pModel->GetSceneName());
    }
}

void Canavar::Engine::RenderingManager::RenderNozzleEffect(NozzleEffect* pEffect, Camera* pCamera, float ifps)
{
    mNozzleEffectShader->Bind();
    mNozzleEffectShader->SetUniformValue("uMaxRadius", pEffect->GetMaxRadius());
    mNozzleEffectShader->SetUniformValue("uMVP", pCamera->GetViewProjectionMatrix() * pEffect->GetWorldTransformation());
    mNozzleEffectShader->SetUniformValue("uZFar", dynamic_cast<PerspectiveCamera*>(pCamera)->GetZFar());
    pEffect->Render(ifps);
    mNozzleEffectShader->Release();
}

void Canavar::Engine::RenderingManager::RenderSky(Camera* pCamera)
{
    mSky->Render(mSkyShader, mSun.get(), pCamera);
}

void Canavar::Engine::RenderingManager::RenderTerrain(Terrain* pTerrain, Camera* pCamera)
{
    const auto& DirectonalLights = mLightManager->GetDirectionalLights();
    const auto NumberOfDirectonalLights = std::min(8, static_cast<int>(DirectonalLights.size()));

    const auto& PointLights = mLightManager->GetPointLightsAround(pCamera->GetWorldPosition(), 20'000.0f);
    const auto NumberOfPointLights = std::min(8, static_cast<int>(PointLights.size()));

    mTerrainShader->Bind();

    mTerrainShader->SetUniformValue("uViewProjectionMatrix", pCamera->GetViewProjectionMatrix());
    mTerrainShader->SetUniformValue("uPreviousViewProjectionMatrix", mPreviousViewProjectionMatrix);
    mTerrainShader->SetUniformValue("uLightViewProjectionMatrix", mShadowMappingRenderer->GetLightViewProjectionMatrix());
    mTerrainShader->SetUniformValue("uZFar", dynamic_cast<PerspectiveCamera*>(pCamera)->GetZFar());
    mTerrainShader->SetUniformValue("uCameraPosition", pCamera->GetWorldPosition());
    mTerrainShader->SetUniformValue("uHaze.enabled", mHaze->GetEnabled());
    mTerrainShader->SetUniformValue("uHaze.color", mHaze->GetColor());
    mTerrainShader->SetUniformValue("uHaze.density", mHaze->GetDensity());
    mTerrainShader->SetUniformValue("uHaze.gradient", mHaze->GetGradient());
    mTerrainShader->SetUniformValue("uNumberOfDirectionalLights", NumberOfDirectonalLights);
    mTerrainShader->SetUniformValue("uNumberOfPointLights", NumberOfPointLights);
    mTerrainShader->SetSampler("uShadow.map", SHADOW_MAP_TEXTURE_UNIT, mShadowMappingRenderer->GetShadowMapTexture());
    mTerrainShader->SetUniformValue("uShadow.enabled", mShadowsEnabled);
    mTerrainShader->SetUniformValue("uShadow.bias", mShadowBias);
    mTerrainShader->SetUniformValue("uShadow.samples", mShadowSamples);

    for (int i = 0; i < NumberOfDirectonalLights; i++)
    {
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].direction", DirectonalLights[i]->GetDirection());
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].color", DirectonalLights[i]->GetColor());
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].ambient", DirectonalLights[i]->GetAmbient());
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].diffuse", DirectonalLights[i]->GetDiffuse());
        mTerrainShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].specular", DirectonalLights[i]->GetSpecular());
    }

    for (int i = 0; i < NumberOfPointLights; i++)
    {
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].color", PointLights[i]->GetColor());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].position", PointLights[i]->GetWorldPosition());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].ambient", PointLights[i]->GetAmbient());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].diffuse", PointLights[i]->GetDiffuse());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].specular", PointLights[i]->GetSpecular());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].constant", PointLights[i]->GetConstant());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].linear", PointLights[i]->GetLinear());
        mTerrainShader->SetUniformValue("uPointLights[" + QString::number(i) + "].quadratic", PointLights[i]->GetQuadratic());
    }

    mTerrainShader->Release();
    mTerrain->Render(mTerrainShader, pCamera);
}

void Canavar::Engine::RenderingManager::SetUniforms(Camera* pCamera)
{
    SetCommonUniforms(mModelShader, pCamera);
    SetDirectionalLights(mModelShader);
}

void Canavar::Engine::RenderingManager::SetCommonUniforms(Shader* pShader, Camera* pCamera)
{
    pShader->Bind();

    pShader->SetUniformValue("uZFar", dynamic_cast<PerspectiveCamera*>(pCamera)->GetZFar());
    pShader->SetUniformValue("uHaze.enabled", mHaze->GetEnabled());
    pShader->SetUniformValue("uHaze.color", mHaze->GetColor());
    pShader->SetUniformValue("uHaze.density", mHaze->GetDensity());
    pShader->SetUniformValue("uHaze.gradient", mHaze->GetGradient());
    pShader->SetUniformValue("uCameraPosition", pCamera->GetWorldPosition());
    pShader->SetUniformValue("uViewProjectionMatrix", pCamera->GetViewProjectionMatrix());
    pShader->SetUniformValue("uPreviousViewProjectionMatrix", mPreviousViewProjectionMatrix);
    pShader->Release();
}

void Canavar::Engine::RenderingManager::SetDirectionalLights(Shader* pShader)
{
    const auto& lights = mLightManager->GetDirectionalLights();
    const auto numberOfLights = std::min(8, static_cast<int>(lights.size()));

    pShader->Bind();
    pShader->SetUniformValue("uNumberOfDirectionalLights", numberOfLights);

    for (int i = 0; i < numberOfLights; i++)
    {
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].direction", lights[i]->GetDirection());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].color", lights[i]->GetColor());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].ambient", lights[i]->GetAmbient());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].diffuse", lights[i]->GetDiffuse());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].specular", lights[i]->GetSpecular());

        if (pShader == mModelShader)
        {
            pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].radiance", lights[i]->GetRadiance());
        }
    }

    pShader->Release();
}

void Canavar::Engine::RenderingManager::SetPointLights(Shader* pShader, Object* pObject)
{
    const auto& lights = mLightManager->GetPointLightsAround(pObject->GetWorldPosition(), 100'000.0f);
    const auto numberOfPointLights = std::min(8, static_cast<int>(lights.size()));

    pShader->Bind();
    pShader->SetUniformValue("uNumberOfPointLights", numberOfPointLights);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].color", lights[i]->GetColor());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].position", lights[i]->GetWorldPosition());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].ambient", lights[i]->GetAmbient());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].diffuse", lights[i]->GetDiffuse());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].specular", lights[i]->GetSpecular());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].constant", lights[i]->GetConstant());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].linear", lights[i]->GetLinear());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].quadratic", lights[i]->GetQuadratic());
    }

    pShader->Release();
}

void Canavar::Engine::RenderingManager::ResizeFramebuffers()
{
    for (const auto type : FBO_TYPES)
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

        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F);
        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F);
        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F);
        mFramebuffers[type]->addColorAttachment(mWidth, mHeight, GL_RGBA32F);

        mFramebuffers[type]->bind();
        glDrawBuffers(NUMBER_OF_FBO_ATTACHMENTS, FBO_ATTACHMENTS);
        mFramebuffers[type]->release();
    }

    for (const auto type : { MotionBlur, Aces, Cinematic, Temp })
    {
        mFramebuffers[type] = new QOpenGLFramebufferObject(mWidth, mHeight, mFramebufferFormats[type]);
    }
}

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
