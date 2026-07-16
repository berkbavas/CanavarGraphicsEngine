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

    // Set extra color attachments for multisample framebuffer
    // OutFragLocalPosition, OutFragWorldPosition, OutNodeInfo
    mFramebufferExtraColorAttachments[Multisample] = { GL_RGBA32F, GL_RGBA32F, GL_RGBA32F };

    mFramebufferFormats[Singlesample].setSamples(0);
    mFramebufferFormats[Singlesample].setMipmap(false);
    mFramebufferFormats[Singlesample].setInternalTextureFormat(QOpenGLTexture::RGBA32F);

    // Set extra color attachments for singlesample framebuffer
    // OutFragLocalPosition, OutFragWorldPosition, OutNodeInfo
    mFramebufferExtraColorAttachments[Singlesample] = { GL_RGBA32F, GL_RGBA32F, GL_RGBA32F };
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

    CreateFreeCameraIfAbsent();
    CreateDirectionalLights();

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

    QOpenGLFramebufferObject::bindDefault();
    glViewport(0, 0, mWidth * mDevicePixelRatio, mHeight * mDevicePixelRatio);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    mScreenShader->Bind();
    mScreenShader->SetSampler("uColorTexture", 0, mFramebuffers[Singlesample]->GetTexture());
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
        FreeCamera *pCamera = mNodeManager->CreateCamera<FreeCamera>();
        mCameraManager->SetActiveCamera(pCamera);
    }
}

void Canavar::Engine::Renderer::CreateDirectionalLights()
{
    QVector3D Directions[6] = { UP, DOWN, LEFT, RIGHT, FORWARD, BACKWARD };

    for (const QVector3D &Direction : Directions)
    {
        DirectionalLight *pLight = mNodeManager->CreateLight<DirectionalLight>();
        pLight->SetAmbient(0.125f);
        pLight->SetDiffuse(0.5f);
        pLight->SetRadiance(4.0f);
        pLight->SetDirection(Direction);
    }
}
