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
    mPrimitiveModelRenderer = std::make_unique<PrimitiveModelRenderer>(this);
    mBoundingBoxRenderer = std::make_unique<BoundingBoxRenderer>(this);

    mManagers.append(mNodeManager.get());
    mManagers.append(mLightManager.get());
    mManagers.append(mCameraManager.get());
    mManagers.append(mTexturedModelRenderer.get());
    mManagers.append(mPrimitiveModelRenderer.get());
    mManagers.append(mBoundingBoxRenderer.get());

    mGizmo = std::make_unique<Gizmo>(this);

    AddEventReceiver(mCameraManager.get()); // First to receive events to handle camera movement
    AddEventReceiver(mGizmo.get());         // Gizmo should receive events after the camera manager

    // FBOs
    mFramebufferFormats[Multisample].setSamples(NUM_SAMPLES);
    mFramebufferFormats[Multisample].setMipmap(true);
    mFramebufferFormats[Multisample].setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    mFramebufferExtraColorAttachments[Multisample] = { GL_RGBA32F, GL_RGBA32F, GL_RGBA32F }; // World, Local, NodeInfo

    mFramebufferFormats[Singlesample].setSamples(0);
    mFramebufferFormats[Singlesample].setMipmap(false);
    mFramebufferFormats[Singlesample].setInternalTextureFormat(QOpenGLTexture::RGBA32F);
    mFramebufferExtraColorAttachments[Singlesample] = { GL_RGBA32F, GL_RGBA32F, GL_RGBA32F }; // World, Local, NodeInfo

    mFramebufferFormats[Ping].setSamples(0);
    mFramebufferFormats[Ping].setInternalTextureFormat(QOpenGLTexture::RGBA32F);

    mFramebufferFormats[Pong].setSamples(0);
    mFramebufferFormats[Pong].setInternalTextureFormat(QOpenGLTexture::RGBA32F);
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
    mPostProcessEffectEnabled[PostProcessEffectType::Sharpen] = true;

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

    CreateDirectionalLights();
    CreateGlobalNodes();

    emit Initialized();

    CreatePersecutorCameraIfAbsent();
    CreateFreeCameraIfAbsent();
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

    // Update all managers before rendering
    for (Manager *pManager : mManagers)
    {
        pManager->Update(Ifps);
    }

    emit Updated(Ifps);

    // Render the scene to the multisample framebuffer using the active camera
    RenderToFramebuffer(mFramebuffers[Multisample].get(), mCameraManager->GetActiveCamera());

    // Blit multisample framebuffer to singlesample framebuffer
    mFramebuffers[Multisample]->BlitColorBufferTo(mFramebuffers[Singlesample].get(), GL_COLOR_ATTACHMENT0);
    mFramebuffers[Multisample]->BlitColorBufferTo(mFramebuffers[Singlesample].get(), GL_COLOR_ATTACHMENT1);
    mFramebuffers[Multisample]->BlitColorBufferTo(mFramebuffers[Singlesample].get(), GL_COLOR_ATTACHMENT2);
    mFramebuffers[Multisample]->BlitColorBufferTo(mFramebuffers[Singlesample].get(), GL_COLOR_ATTACHMENT3);

    // Apply post-processing effects to the rendered scene.
    // Note: The order of effects in the mPostProcessEffects map determines the order in which they are applied.
    // Note: The final result of the post-processing pipeline will be in the Pong framebuffer.
    ApplyPostProcessEffects();

    // Render the final result to the default framebuffer (screen)
    QOpenGLFramebufferObject::bindDefault();
    glViewport(0, 0, mWidth * mDevicePixelRatio, mHeight * mDevicePixelRatio);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mScreenShader->Bind();
    mScreenShader->SetSampler("uColorTexture", 0, mFramebuffers[Pong]->GetTexture());
    mQuad->Render();
    mScreenShader->Unbind();

    // QPainter rendering should be done after the main render pass to ensure it appears on top of everything else.
    for (Manager *pManager : mManagers)
    {
        pManager->Paint(mOpenGLWidget);
    }

    // ImGui rendering should be done after the main render pass to ensure it appears on top of everything else.
    emit PostRender(Ifps);
}

void Canavar::Engine::Renderer::RenderToFramebuffer(Framebuffer *pFramebuffer, PerspectiveCamera *pCamera)
{
    // Setup OpenGL state for rendering
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    pFramebuffer->Bind();

    // Clear the framebuffer with the specified clear color and depth/stencil buffers
    glViewport(0, 0, pFramebuffer->GetWidth(), pFramebuffer->GetHeight());
    glClearColor(mClearColor.x(), mClearColor.y(), mClearColor.z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Render the sky as it is the background of the scene. The sky should be rendered first to ensure it appears behind all other objects.
    mSky->Render(pCamera, mSun);

    // Render the terrain
    mTerrain->Render();

    for (Manager *pManager : mManagers)
    {
        pManager->Render(RenderPass::Opaque, pCamera);
    }

    // Disable depth testing for overlay rendering in the opaque pass
    glDisable(GL_DEPTH_TEST);

    // Render overlays for the opaque pass
    for (Manager *pManager : mManagers)
    {
        pManager->RenderOverlay(RenderPass::Opaque, pCamera);
    }

    // Re-enable depth testing for the transparent pass
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE); // Disable depth writing for transparent objects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render the transparent pass for all managers
    for (Manager *pManager : mManagers)
    {
        pManager->Render(RenderPass::Transparent, pCamera);
    }

    // Disable depth testing for overlay rendering in the transparent pass
    glDisable(GL_DEPTH_TEST);

    // Render overlays for the transparent pass
    for (Manager *pManager : mManagers)
    {
        pManager->RenderOverlay(RenderPass::Transparent, pCamera);
    }

    // Unbind the multisample framebuffer to prepare for blitting to the singlesample framebuffer
    pFramebuffer->Unbind();
}

const QMap<QString, Canavar::Engine::ScenePtr> &Canavar::Engine::Renderer::GetScenes() const
{
    return mTexturedModelRenderer->GetScenes();
}

Canavar::Engine::Scene* Canavar::Engine::Renderer::GetSceneByName(const QString &Name) const
{
    return mTexturedModelRenderer->GetSceneByName(Name);
}

void Canavar::Engine::Renderer::OnKeyPressed(QKeyEvent *pEvent)
{
    for (EventThief *pEventThief : mEventThieves)
    {
        if (pEventThief->WantsKeyboardCapture())
        {
            return; // Event has been stolen, do not propagate further
        }
    }

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
    for (EventThief *pEventThief : mEventThieves)
    {
        if (pEventThief->WantsKeyboardCapture())
        {
            return; // Event has been stolen, do not propagate further
        }
    }

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
    for (EventThief *pEventThief : mEventThieves)
    {
        if (pEventThief->WantsMouseCapture())
        {
            return; // Event has been stolen, do not propagate further
        }
    }

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
    for (EventThief *pEventThief : mEventThieves)
    {
        if (pEventThief->WantsMouseCapture())
        {
            return; // Event has been stolen, do not propagate further
        }
    }

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
    for (EventThief *pEventThief : mEventThieves)
    {
        if (pEventThief->WantsMouseCapture())
        {
            return; // Event has been stolen, do not propagate further
        }
    }

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
    for (EventThief *pEventThief : mEventThieves)
    {
        if (pEventThief->WantsMouseCapture())
        {
            return; // Event has been stolen, do not propagate further
        }
    }

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
        // Do not return early; allow all event receivers to handle the leave event
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
    if (mNodeManager->FindNodeByType<FreeCamera>() == nullptr)
    {
        FreeCamera *pCamera = mNodeManager->CreateNode<FreeCamera>();
        mCameraManager->SetActiveCamera(pCamera);
    }
}

void Canavar::Engine::Renderer::CreatePersecutorCameraIfAbsent()
{
    if (mNodeManager->FindNodeByType<PersecutorCamera>() == nullptr)
    {
        PersecutorCamera *pCamera = mNodeManager->CreateNode<PersecutorCamera>();
        mCameraManager->SetActiveCamera(pCamera);
    }
}

void Canavar::Engine::Renderer::CreateDirectionalLights()
{
    // Create a primary directional light (the sun) and additional directional lights in various directions for ambient lighting
    mSun = mNodeManager->CreateNode<DirectionalLight>();
    mSun->SetDirection(QVector3D(-1, -1, -1).normalized());

    // QVector3D Directions[5] = { UP, LEFT, RIGHT, FORWARD, BACKWARD };

    // for (const QVector3D &Direction : Directions)
    // {
    //     DirectionalLight *pLight = mNodeManager->CreateNode<DirectionalLight>();
    //     pLight->SetAmbient(0.125f);
    //     pLight->SetDiffuse(0.5f);
    //     pLight->SetRadiance(4.0f);
    //     pLight->SetDirection(Direction);
    // }
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
    SupplyPerFrameData();

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

    // Pong now contains the final result after all post-processing effects have been applied.
}

void Canavar::Engine::Renderer::SupplyPerFrameData()
{
    const auto Textures = mFramebuffers[Singlesample]->GetTextures();

    if (Textures.size() > 2)
    {
        mDepthOfFieldEffect->SetWorldPositionTexture(Textures[2]); // COLOR_ATTACHMENT2 = World Position
    }

    if (PerspectiveCamera *pCamera = mCameraManager->GetActiveCamera())
    {
        mDepthOfFieldEffect->SetCameraPosition(pCamera->GetPosition());
    }
}

Canavar::Engine::LightManager *Canavar::Engine::Renderer::GetLightManager() const
{
    return mLightManager.get();
}

Canavar::Engine::CameraManager *Canavar::Engine::Renderer::GetCameraManager() const
{
    return mCameraManager.get();
}

Canavar::Engine::BoundingBoxRenderer *Canavar::Engine::Renderer::GetBoundingBoxRenderer() const
{
    return mBoundingBoxRenderer.get();
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

Canavar::Engine::Gizmo *Canavar::Engine::Renderer::GetGizmo() const
{
    return mGizmo.get();
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

void Canavar::Engine::Renderer::AddEventThief(EventThief *pEventThief)
{
    mEventThieves.append(pEventThief);
}

void Canavar::Engine::Renderer::RemoveEventThief(EventThief *pEventThief)
{
    mEventThieves.removeAll(pEventThief);
}
