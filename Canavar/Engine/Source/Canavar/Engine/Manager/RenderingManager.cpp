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

    mQuad = new Quad;

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
}

void Canavar::Engine::RenderingManager::Shutdown()
{
    DestroyFramebuffers();

    delete mQuad;
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
    mQuad->Render();
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
    mQuad->Render();
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
    mQuad->Render();
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

    SetUniforms(mActiveCamera);

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
                RenderNozzleEffect(pEffect.get(), pCamera);
            }
            else if (LightningStrikeBasePtr pLightning = std::dynamic_pointer_cast<LightningStrikeBase>(pObject))
            {
                pLightning->Render(pCamera, mLightningStrikeShader, mLineShader, mIfps);
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
        mModelShader->SetUniformValue("uShadow.enabled", mShadowMappingRenderer->GetShadowsEnabled());
        mModelShader->SetUniformValue("uShadow.bias", mShadowMappingRenderer->GetShadowBias());
        mModelShader->SetUniformValue("uShadow.samples", mShadowMappingRenderer->GetShadowSamples());
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
    mModelShader->SetUniformValue("uShadow.enabled", mShadowMappingRenderer->GetShadowsEnabled());
    mModelShader->SetUniformValue("uShadow.bias", mShadowMappingRenderer->GetShadowBias());
    mModelShader->SetUniformValue("uShadow.samples", mShadowMappingRenderer->GetShadowSamples());

    if (const auto pScene = mNodeManager->GetScene(pModel))
    {
        pScene->Render(pModel, mModelShader, RenderPass);
    }
    else
    {
        LOG_FATAL("RenderingManager::RenderModel: Model data is not found for this model: {}", pModel->GetSceneName());
    }
}

void Canavar::Engine::RenderingManager::RenderNozzleEffect(NozzleEffect* pEffect, PerspectiveCamera* pCamera)
{
    mNozzleEffectShader->Bind();
    mNozzleEffectShader->SetUniformValue("uMaxRadius", pEffect->GetMaxRadius());
    mNozzleEffectShader->SetUniformValue("uMVP", pCamera->GetViewProjectionMatrix() * pEffect->GetWorldTransformation());
    mNozzleEffectShader->SetUniformValue("uZFar", pCamera->GetZFar());
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
    mTerrainShader->SetUniformValue("uZFar", pCamera->GetZFar());
    mTerrainShader->SetUniformValue("uCameraPosition", pCamera->GetWorldPosition());
    mTerrainShader->SetUniformValue("uHaze.enabled", mHaze->GetEnabled());
    mTerrainShader->SetUniformValue("uHaze.color", mHaze->GetColor());
    mTerrainShader->SetUniformValue("uHaze.density", mHaze->GetDensity());
    mTerrainShader->SetUniformValue("uHaze.gradient", mHaze->GetGradient());
    mTerrainShader->SetUniformValue("uNumberOfDirectionalLights", NumberOfDirectonalLights);
    mTerrainShader->SetUniformValue("uNumberOfPointLights", NumberOfPointLights);
    mTerrainShader->SetSampler("uShadow.map", SHADOW_MAP_TEXTURE_UNIT, mShadowMappingRenderer->GetShadowMapTexture());
    mTerrainShader->SetUniformValue("uShadow.enabled", mShadowMappingRenderer->GetShadowsEnabled());
    mTerrainShader->SetUniformValue("uShadow.bias", mShadowMappingRenderer->GetShadowBias());
    mTerrainShader->SetUniformValue("uShadow.samples", mShadowMappingRenderer->GetShadowSamples());

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

void Canavar::Engine::RenderingManager::SetUniforms(PerspectiveCamera* pCamera)
{
    SetCommonUniforms(mModelShader, pCamera);
    SetDirectionalLights(mModelShader);
}

void Canavar::Engine::RenderingManager::SetCommonUniforms(Shader* pShader, PerspectiveCamera* pCamera)
{
    pShader->Bind();

    pShader->SetUniformValue("uZFar", pCamera->GetZFar());
    pShader->SetUniformValue("uMeshSelectionEnabled", static_cast<int>(mMeshSelectionEnabled));
    pShader->SetUniformValue("uHaze.enabled", mHaze->GetEnabled());
    pShader->SetUniformValue("uHaze.color", mHaze->GetColor());
    pShader->SetUniformValue("uHaze.density", mHaze->GetDensity());
    pShader->SetUniformValue("uHaze.gradient", mHaze->GetGradient());
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
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].direction", Lights[i]->GetDirection());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].color", Lights[i]->GetColor());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].ambient", Lights[i]->GetAmbient());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].diffuse", Lights[i]->GetDiffuse());
        pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].specular", Lights[i]->GetSpecular());

        if (pShader == mModelShader)
        {
            pShader->SetUniformValue("uDirectionalLights[" + QString::number(i) + "].radiance", Lights[i]->GetRadiance());
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
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].color", Lights[i]->GetColor());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].position", Lights[i]->GetWorldPosition());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].ambient", Lights[i]->GetAmbient());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].diffuse", Lights[i]->GetDiffuse());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].specular", Lights[i]->GetSpecular());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].constant", Lights[i]->GetConstant());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].linear", Lights[i]->GetLinear());
        pShader->SetUniformValue("uPointLights[" + QString::number(i) + "].quadratic", Lights[i]->GetQuadratic());
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
