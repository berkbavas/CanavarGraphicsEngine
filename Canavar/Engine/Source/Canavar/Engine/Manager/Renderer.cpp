#include "Renderer.h"

Canavar::Engine::Renderer::Renderer(OpenGLWidget *pOpenGLWidget)
    : mOpenGLWidget(pOpenGLWidget)
    , mRenderingContext(pOpenGLWidget)
{
    connect(mOpenGLWidget, &OpenGLWidget::Initialized, this, &Renderer::Initialize);
    connect(mOpenGLWidget, &OpenGLWidget::Resized, this, &Renderer::Resize);
    connect(mOpenGLWidget, &OpenGLWidget::Render, this, &Renderer::Render);
    connect(mOpenGLWidget, &OpenGLWidget::KeyPressed, this, &Renderer::OnKeyPressed);
    connect(mOpenGLWidget, &OpenGLWidget::KeyReleased, this, &Renderer::OnKeyReleased);
    connect(mOpenGLWidget, &OpenGLWidget::MousePressed, this, &Renderer::OnMousePressed);
    connect(mOpenGLWidget, &OpenGLWidget::MouseReleased, this, &Renderer::OnMouseReleased);
    connect(mOpenGLWidget, &OpenGLWidget::MouseMoved, this, &Renderer::OnMouseMoved);
    connect(mOpenGLWidget, &OpenGLWidget::WheelMoved, this, &Renderer::OnWheelMoved);
    connect(mOpenGLWidget, &OpenGLWidget::LeaveEvent, this, &Renderer::OnLeaveEvent);

    mNodeManager = std::make_unique<NodeManager>(this);
    mLightManager = std::make_unique<LightManager>();
    mCameraManager = std::make_unique<CameraManager>();
    mTexturedModelRenderer = std::make_unique<TexturedModelRenderer>(this);

    mManagers.append(mNodeManager.get());
    mManagers.append(mLightManager.get());
    mManagers.append(mCameraManager.get());
    mManagers.append(mTexturedModelRenderer.get());

    AddEventReceiver(mCameraManager.get());

    mFramebufferFormats[Multisample].setSamples(NUM_SAMPLES);
    mFramebufferFormats[Multisample].setMipmap(true);
    mFramebufferFormats[Multisample].setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    mFramebufferExtraColorAttachments[Multisample] = { GL_RGBA32F, GL_RGBA32F, GL_RGBA32F };

    mFramebufferFormats[Singlesample].setSamples(0);
    mFramebufferFormats[Singlesample].setMipmap(false);
    mFramebufferFormats[Singlesample].setInternalTextureFormat(QOpenGLTexture::RGBA32F);
    mFramebufferExtraColorAttachments[Singlesample] = { GL_RGBA32F, GL_RGBA32F, GL_RGBA32F };

    mFramebufferFormats[Ping].setSamples(0);
    mFramebufferFormats[Ping].setInternalTextureFormat(QOpenGLTexture::RGBA32F);

    mFramebufferFormats[Pong].setSamples(0);
    mFramebufferFormats[Pong].setInternalTextureFormat(QOpenGLTexture::RGBA32F);

}

Canavar::Engine::LightManager *Canavar::Engine::Renderer::GetLightManager() const
{
    return mLightManager.get();
}

Canavar::Engine::CameraManager *Canavar::Engine::Renderer::GetCameraManager() const
{
    return mCameraManager.get();
}

Canavar::Engine::TexturedModelRenderer *Canavar::Engine::Renderer::GetTexturedModelRenderer() const
{
    return mTexturedModelRenderer.get();
}

Canavar::Engine::Sky *Canavar::Engine::Renderer::GetSky() const
{
    return mSky;
}

Canavar::Engine::Haze *Canavar::Engine::Renderer::GetHaze() const
{
    return mHaze;
}

Canavar::Engine::Terrain *Canavar::Engine::Renderer::GetTerrain() const
{
    return mTerrain;
}

Canavar::Engine::AcesEffect *Canavar::Engine::Renderer::GetAcesEffect() const
{
    return mAcesEffect.get();
}

Canavar::Engine::DepthOfFieldEffect *Canavar::Engine::Renderer::GetDepthOfFieldEffect() const
{
    return mDepthOfFieldEffect.get();
}

Canavar::Engine::FxaaEffect *Canavar::Engine::Renderer::GetFxaaEffect() const
{
    return mFxaaEffect.get();
}

Canavar::Engine::ColorGradingEffect *Canavar::Engine::Renderer::GetColorGradingEffect() const
{
    return mColorGradingEffect.get();
}

Canavar::Engine::SharpenEffect *Canavar::Engine::Renderer::GetSharpenEffect() const
{
    return mSharpenEffect.get();
}

Canavar::Engine::ChromaticAberrationEffect *Canavar::Engine::Renderer::GetChromaticAberrationEffect() const
{
    return mChromaticAberrationEffect.get();
}

Canavar::Engine::LensDistortionEffect *Canavar::Engine::Renderer::GetLensDistortionEffect() const
{
    return mLensDistortionEffect.get();
}

Canavar::Engine::CinematicEffect *Canavar::Engine::Renderer::GetCinematicEffect() const
{
    return mCinematicEffect.get();
}

bool Canavar::Engine::Renderer::GetPostProcessEffectEnabled(PostProcessEffectType Type) const
{
    auto It = mPostProcessEffectEnabled.find(Type);
    return (It != mPostProcessEffectEnabled.end()) && It->second;
}

void Canavar::Engine::Renderer::SetPostProcessEffectEnabled(PostProcessEffectType Type, bool Enabled)
{
    mPostProcessEffectEnabled[Type] = Enabled;
}

Canavar::Engine::PerspectiveCamera *Canavar::Engine::Renderer::GetActiveCamera() const
{
    return mCameraManager->GetActiveCamera();
}

Canavar::Engine::OpenGLWidget *Canavar::Engine::Renderer::GetOpenGLWidget() const
{
    return mOpenGLWidget;
}

Canavar::Engine::NodeManager *Canavar::Engine::Renderer::GetNodeManager() const
{
    return mNodeManager.get();
}

void Canavar::Engine::Renderer::AddEventReceiver(EventReceiver *pEventReceiver)
{
    mEventReceivers.append(pEventReceiver);
}

void Canavar::Engine::Renderer::RemoveEventReceiver(EventReceiver *pEventReceiver)
{
    mEventReceivers.removeAll(pEventReceiver);
}

void Canavar::Engine::Renderer::Initialize()
{
    Q_INIT_RESOURCE(Engine);
    Q_INIT_RESOURCE(EngineBigObjects);

    qInstallMessageHandler(Logger::QtMessageOutputCallback);

    initializeOpenGLFunctions();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    for (Manager *pManager : mManagers)
    {
        pManager->Initialize();
    }

    for (Manager *pManager : mManagers)
    {
        pManager->PostInitialize();
    }

    for (const auto &[Type, Format] : mFramebufferFormats)
    {
        mFramebuffers[Type] = std::make_unique<Framebuffer>(mWidth, mHeight, Format, mFramebufferExtraColorAttachments[Type]);
    }

    mScreenShader = std::make_unique<Shader>("Screen Shader");
    mScreenShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mScreenShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Screen.frag");
    mScreenShader->Initialize();

    mQuad = std::make_unique<Quad>();

    // Post-process effects (order determines pipeline execution)
    mAcesEffect = std::make_unique<AcesEffect>();
    mPostProcessEffects[PostProcessEffectType::Aces] = mAcesEffect.get();
    mPostProcessEffectEnabled[PostProcessEffectType::Aces] = false;

    mDepthOfFieldEffect = std::make_unique<DepthOfFieldEffect>();
    mPostProcessEffects[PostProcessEffectType::DepthOfField] = mDepthOfFieldEffect.get();
    mPostProcessEffectEnabled[PostProcessEffectType::DepthOfField] = false;

    mColorGradingEffect = std::make_unique<ColorGradingEffect>();
    mPostProcessEffects[PostProcessEffectType::ColorGrading] = mColorGradingEffect.get();
    mPostProcessEffectEnabled[PostProcessEffectType::ColorGrading] = false;

    mSharpenEffect = std::make_unique<SharpenEffect>();
    mPostProcessEffects[PostProcessEffectType::Sharpen] = mSharpenEffect.get();
    mPostProcessEffectEnabled[PostProcessEffectType::Sharpen] = false;

    mFxaaEffect = std::make_unique<FxaaEffect>();
    mPostProcessEffects[PostProcessEffectType::Fxaa] = mFxaaEffect.get();
    mPostProcessEffectEnabled[PostProcessEffectType::Fxaa] = false;

    mChromaticAberrationEffect = std::make_unique<ChromaticAberrationEffect>();
    mPostProcessEffects[PostProcessEffectType::ChromaticAberration] = mChromaticAberrationEffect.get();
    mPostProcessEffectEnabled[PostProcessEffectType::ChromaticAberration] = false;

    mLensDistortionEffect = std::make_unique<LensDistortionEffect>();
    mPostProcessEffects[PostProcessEffectType::LensDistortion] = mLensDistortionEffect.get();
    mPostProcessEffectEnabled[PostProcessEffectType::LensDistortion] = false;

    mCinematicEffect = std::make_unique<CinematicEffect>();
    mPostProcessEffects[PostProcessEffectType::Cinematic] = mCinematicEffect.get();
    mPostProcessEffectEnabled[PostProcessEffectType::Cinematic] = true;

    CreateFreeCameraIfAbsent();
    CreateDirectionalLights();
    CreateGlobalNodes();

    emit Initialized();
}

void Canavar::Engine::Renderer::Resize(int Width, int Height)
{
    for (Manager *pManager : mManagers)
    {
        pManager->Resize(Width, Height);
    }

    ResizeFramebuffers(Width, Height);

    emit Resized(Width, Height);
}

void Canavar::Engine::Renderer::Render(float Ifps)
{
    mDevicePixelRatio = mRenderingContext->GetDevicePixelRatio();

    for (Manager *pManager : mManagers)
    {
        pManager->Update(Ifps);
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    mFramebuffers[Multisample]->Bind();

    glViewport(0, 0, mFramebuffers[Multisample]->GetWidth(), mFramebuffers[Multisample]->GetHeight());
    glClearColor(mClearColor.x(), mClearColor.y(), mClearColor.z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mSky->Render(mCameraManager->GetActiveCamera(), mSun);

    mTerrain->Render();

    for (Manager *pManager : mManagers)
    {
        pManager->Render(RenderPass::Opaque);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for (Manager *pManager : mManagers)
    {
        pManager->Render(RenderPass::Transparent);
    }

    mFramebuffers[Multisample]->Unbind();

    // Blit multisample framebuffer to singlesample framebuffer
    mFramebuffers[Multisample]->BlitColorBufferTo(mFramebuffers[Singlesample].get(), GL_COLOR_ATTACHMENT0);
    mFramebuffers[Multisample]->BlitColorBufferTo(mFramebuffers[Singlesample].get(), GL_COLOR_ATTACHMENT1);
    mFramebuffers[Multisample]->BlitColorBufferTo(mFramebuffers[Singlesample].get(), GL_COLOR_ATTACHMENT2);
    mFramebuffers[Multisample]->BlitColorBufferTo(mFramebuffers[Singlesample].get(), GL_COLOR_ATTACHMENT3);

    ApplyPostProcessEffects(); // Pong contains the final result after all post-processing effects have been applied.

    QOpenGLFramebufferObject::bindDefault();
    glViewport(0, 0, mWidth * mDevicePixelRatio, mHeight * mDevicePixelRatio);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    mScreenShader->Bind();
    mScreenShader->SetSampler("uColorTexture", 0, mFramebuffers[Pong]->GetTexture());
    mQuad->Render();
    mScreenShader->Unbind();

    emit CanRenderOverlay(Ifps); // ImGui rendering should be done after the main render pass to ensure it appears on top of everything else.

    for (Manager *pManager : mManagers)
    {
        pManager->Paint(mOpenGLWidget);
    }
}

void Canavar::Engine::Renderer::OnKeyPressed(QKeyEvent *pEvent)
{
    for (EventReceiver *pEventReceiver : mEventReceivers)
    {
        if (pEventReceiver->OnKeyPressed(pEvent))
        {
            return;
        }
    }
}

void Canavar::Engine::Renderer::OnKeyReleased(QKeyEvent *pEvent)
{
    for (EventReceiver *pEventReceiver : mEventReceivers)
    {
        if (pEventReceiver->OnKeyReleased(pEvent))
        {
            return;
        }
    }
}

void Canavar::Engine::Renderer::OnMousePressed(QMouseEvent *pEvent)
{
    for (EventReceiver *pEventReceiver : mEventReceivers)
    {
        if (pEventReceiver->OnMousePressed(pEvent))
        {
            return;
        }
    }
}

void Canavar::Engine::Renderer::OnMouseReleased(QMouseEvent *pEvent)
{
    for (EventReceiver *pEventReceiver : mEventReceivers)
    {
        if (pEventReceiver->OnMouseReleased(pEvent))
        {
            return;
        }
    }
}

void Canavar::Engine::Renderer::OnMouseMoved(QMouseEvent *pEvent)
{
    for (EventReceiver *pEventReceiver : mEventReceivers)
    {
        if (pEventReceiver->OnMouseMoved(pEvent))
        {
            return;
        }
    }
}

void Canavar::Engine::Renderer::OnWheelMoved(QWheelEvent *pEvent)
{
    for (EventReceiver *pEventReceiver : mEventReceivers)
    {
        if (pEventReceiver->OnWheelMoved(pEvent))
        {
            return;
        }
    }
}

void Canavar::Engine::Renderer::OnLeaveEvent(QEvent *pEvent)
{
    for (EventReceiver *pEventReceiver : mEventReceivers)
    {
        pEventReceiver->OnLeaveEvent(pEvent);
    }
}

void Canavar::Engine::Renderer::ResizeFramebuffers(int Width, int Height)
{
    mWidth = Width;
    mHeight = Height;

    for (const auto &[Type, pFramebuffer] : mFramebuffers)
    {
        pFramebuffer->Resize(Width, Height);
    }
}

void Canavar::Engine::Renderer::CreateFreeCameraIfAbsent()
{
    if (mCameraManager->GetActiveCamera() == nullptr)
    {
        FreeCamera *pCamera = mNodeManager->CreateNode<FreeCamera>();
        mCameraManager->SetActiveCamera(pCamera);
    }
}

void Canavar::Engine::Renderer::CreateDirectionalLights()
{
    mSun = mNodeManager->CreateNode<DirectionalLight>();
    mSun->SetDirection(QVector3D(-1, -1, -1).normalized());

    QVector3D Directions[5] = { UP, LEFT, RIGHT, FORWARD, BACKWARD };

    for (const QVector3D &Direction : Directions)
    {
        DirectionalLight *pLight = mNodeManager->CreateNode<DirectionalLight>();
        pLight->SetAmbient(0.125f);
        pLight->SetDiffuse(0.5f);
        pLight->SetRadiance(4.0f);
        pLight->SetDirection(Direction);
    }
}

void Canavar::Engine::Renderer::CreateGlobalNodes()
{
    mSky = mNodeManager->CreateNode<Sky>();
    mHaze = mNodeManager->CreateNode<Haze>();
    mTerrain = mNodeManager->CreateNode<Terrain>(this);
}

void Canavar::Engine::Renderer::ApplyPostProcessEffects()
{
    // Singlesample -> Pong
    mFramebuffers[Singlesample]->BlitColorBufferTo(mFramebuffers[Pong].get(), GL_COLOR_ATTACHMENT0);

    // Supply per-frame depth data to the DOF effect before the pipeline runs
    {
        const auto Textures = mFramebuffers[Singlesample]->GetTextures();
        if (Textures.size() > 2)
        {
            mDepthOfFieldEffect->SetWorldPositionTexture(Textures[2]); // COLOR_ATTACHMENT2 = World Position
        }
        if (PerspectiveCamera* pCamera = mCameraManager->GetActiveCamera())
        {
            mDepthOfFieldEffect->SetCameraPosition(pCamera->GetPosition());
        }
    }

    for (const auto &[EffectType, pEffect] : mPostProcessEffects)
    {
        if (mPostProcessEffectEnabled[EffectType])
        {
            pEffect->ApplyEffect(mFramebuffers[Pong].get(), mFramebuffers[Ping].get());

            // Ping -> Pong
            // Ensure that final result is written to Pong framebuffer for the next effect in the chain
            mFramebuffers[Ping]->BlitColorBufferTo(mFramebuffers[Pong].get(), GL_COLOR_ATTACHMENT0);
        }
    }
}
