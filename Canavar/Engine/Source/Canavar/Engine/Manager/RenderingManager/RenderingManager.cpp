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

    mFramebufferFormats[Multisample].setSamples(4);
    mFramebufferFormats[Multisample].setMipmap(true);
    mFramebufferFormats[Multisample].setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    mFramebufferFormats[Singlesample].setSamples(0);
    mFramebufferFormats[Ping].setSamples(0);
    mFramebufferFormats[Pong].setSamples(0);

    for (const auto type : { Multisample, Singlesample, Ping, Pong })
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
    mBlurShader = mShaderManager->GetShader(ShaderType::Blur);
    mPostProcessShader = mShaderManager->GetShader(ShaderType::PostProcess);
    mNozzleEffectShader = mShaderManager->GetShader(ShaderType::NozzleEffect);
    mLightningStrikeShader = mShaderManager->GetShader(ShaderType::LightningStrike);
    mLineShader = mShaderManager->GetShader(ShaderType::Line);
    mTerrainShader = mShaderManager->GetShader(ShaderType::Terrain);
}

void Canavar::Engine::RenderingManager::Render(float ifps)
{
    mIfps = ifps;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glDisable(GL_BLEND);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    if (mShadowsEnabled)
    {
        mShadowMappingRenderer->Render(ifps);
    }

    mActiveCamera = mCameraManager->GetActiveCamera().get();

    mFramebuffers[Multisample]->bind();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SetUniforms(mActiveCamera);

    RenderSky(mActiveCamera);
    RenderTerrain(mTerrain.get(), mActiveCamera);

    if (mDrawBoundingBoxes)
    {
        mBoundingBoxRenderer->Render(mActiveCamera, ifps);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    RenderObjects(mActiveCamera, ifps);

    if (mCrossSectionEnabled)
    {
        mCrossSectionAnalyzer->RenderPlane();
    }

    mTextRenderer->Render(mActiveCamera);

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

    QOpenGLFramebufferObject::blitFramebuffer(nullptr, mFramebuffers[Multisample], GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // mFramebuffers[Temp2]->bind();
    // glViewport(0, 0, mWidth, mHeight);
    // glClearColor(0, 0, 0, 0);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // mPostProcessShader->Bind();
    // mPostProcessShader->SetUniformValue("uBlurThreshold", mBlurThreshold);
    // mPostProcessShader->SetUniformValue("uMaxSamples", mMaxSamples);
    // mPostProcessShader->SetSampler("uColorTexture", 0, mFramebuffers[Temp1]->texture());
    // mPostProcessShader->SetSampler("uDistanceTexture", 1, mFramebuffers[Singlesample]->textures().at(4));
    // mQuad->Render();
    // mPostProcessShader->Release();
    // mFramebuffers[Temp2]->release();

    // QOpenGLFramebufferObject::blitFramebuffer(nullptr, mFramebuffers[Temp2], GL_COLOR_BUFFER_BIT, GL_LINEAR);

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
    glPolygonOffset(1.0f, 1.0f);

    pFramebuffer->bind();

    glViewport(0, 0, pFramebuffer->width(), pFramebuffer->height());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SetUniforms(pCamera);

    RenderSky(pCamera);
    RenderTerrain(mTerrain.get(), pCamera);

    RenderObjects(pCamera, mIfps);

    mTextRenderer->Render(mActiveCamera);

    // For QPainter
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
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
    const auto& nodes = mNodeManager->GetNodes();

    // std::sort(nodes.begin(), nodes.end(), [&](const NodePtr& left, const NodePtr& right) {
    //     const auto l = std::dynamic_pointer_cast<Object>(left);
    //     const auto r = std::dynamic_pointer_cast<Object>(right);
    //     if (l && r)
    //     {
    //         float dl = (pCamera->GetWorldPosition() - l->GetWorldPosition()).length();
    //         float dr = (pCamera->GetWorldPosition() - r->GetWorldPosition()).length();

    //         return dl > dr;
    //     }

    //     return false;
    // });

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
                RenderModel(pModel, RenderPass::Opaque);
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

    glDepthMask(GL_FALSE);

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
                RenderModel(pModel, RenderPass::Transparent);
            }
        }
    }

    glDepthMask(GL_TRUE);
}

void Canavar::Engine::RenderingManager::RenderModel(ModelPtr pModel, RenderPass renderPass)
{
    SetPointLights(mModelShader, pModel.get());

    mModelShader->Bind();
    mModelShader->SetUniformValue("uModel.color", pModel->GetColor());
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
        pScene->Render(pModel.get(), mModelShader, renderPass);
    }
    else
    {
        LOG_FATAL("RenderingManager::RenderModel: Model data is not found for this model: {}", pModel->GetSceneName().toStdString());
    }

    mModelShader->Release();
}

void Canavar::Engine::RenderingManager::RenderNozzleEffect(NozzleEffectPtr pEffect, Camera* pCamera, float ifps)
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
    const auto& directional_lights = mLightManager->GetDirectionalLights();
    const auto number_of_directional_lights = std::min(8, static_cast<int>(directional_lights.size()));

    const auto& point_lights = mLightManager->GetPointLightsAround(pCamera->GetWorldPosition(), 20'000.0f);
    const auto number_of_point_lights = std::min(8, static_cast<int>(point_lights.size()));

    mTerrainShader->Bind();

    mTerrainShader->SetUniformValue("VP", pCamera->GetViewProjectionMatrix());
    mTerrainShader->SetUniformValue("LVP", mShadowMappingRenderer->GetLightViewProjectionMatrix());
    mTerrainShader->SetUniformValue("z_far", dynamic_cast<PerspectiveCamera*>(pCamera)->GetZFar());
    mTerrainShader->SetUniformValue("eye_world_pos", pCamera->GetWorldPosition());
    mTerrainShader->SetUniformValue("haze.enabled", mHaze->GetEnabled());
    mTerrainShader->SetUniformValue("haze.color", mHaze->GetColor());
    mTerrainShader->SetUniformValue("haze.density", mHaze->GetDensity());
    mTerrainShader->SetUniformValue("haze.gradient", mHaze->GetGradient());
    mTerrainShader->SetUniformValue("number_of_directional_lights", number_of_directional_lights);
    mTerrainShader->SetUniformValue("number_of_point_lights", number_of_point_lights);
    mTerrainShader->SetSampler("shadow.map", SHADOW_MAP_TEXTURE_UNIT, mShadowMappingRenderer->GetShadowMapTexture());
    mTerrainShader->SetUniformValue("shadow.enabled", mShadowsEnabled);
    mTerrainShader->SetUniformValue("shadow.bias", mShadowBias);
    mTerrainShader->SetUniformValue("shadow.samples", mShadowSamples);

    for (int i = 0; i < number_of_directional_lights; i++)
    {
        mTerrainShader->SetUniformValue("directional_lights[" + QString::number(i) + "].direction", directional_lights[i]->GetDirection());
        mTerrainShader->SetUniformValue("directional_lights[" + QString::number(i) + "].color", directional_lights[i]->GetColor());
        mTerrainShader->SetUniformValue("directional_lights[" + QString::number(i) + "].ambient", directional_lights[i]->GetAmbient());
        mTerrainShader->SetUniformValue("directional_lights[" + QString::number(i) + "].diffuse", directional_lights[i]->GetDiffuse());
        mTerrainShader->SetUniformValue("directional_lights[" + QString::number(i) + "].specular", directional_lights[i]->GetSpecular());
    }

    for (int i = 0; i < number_of_point_lights; i++)
    {
        mTerrainShader->SetUniformValue("point_lights[" + QString::number(i) + "].color", point_lights[i]->GetColor());
        mTerrainShader->SetUniformValue("point_lights[" + QString::number(i) + "].position", point_lights[i]->GetWorldPosition());
        mTerrainShader->SetUniformValue("point_lights[" + QString::number(i) + "].ambient", point_lights[i]->GetAmbient());
        mTerrainShader->SetUniformValue("point_lights[" + QString::number(i) + "].diffuse", point_lights[i]->GetDiffuse());
        mTerrainShader->SetUniformValue("point_lights[" + QString::number(i) + "].specular", point_lights[i]->GetSpecular());
        mTerrainShader->SetUniformValue("point_lights[" + QString::number(i) + "].constant", point_lights[i]->GetConstant());
        mTerrainShader->SetUniformValue("point_lights[" + QString::number(i) + "].linear", point_lights[i]->GetLinear());
        mTerrainShader->SetUniformValue("point_lights[" + QString::number(i) + "].quadratic", point_lights[i]->GetQuadratic());
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
    pShader->SetUniformValue("uEnableAces", mEnableAces);
    pShader->SetUniformValue("uExposure", mExposure);
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
    for (const auto type : { Multisample, Singlesample, Ping, Pong })
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

    for (const auto type : { Ping, Pong })
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
