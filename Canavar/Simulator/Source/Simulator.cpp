#include "Simulator.h"

#include <Canavar/Engine/Core/Window.h>

#include <QQmlContext>

Canavar::Simulator::Simulator::Simulator()
{
    mController = new Canavar::Engine::Controller(Canavar::Engine::ContainerMode::Widget);
    mController->AddEventReceiver(this);

    mPfd = new PrimaryFlightData;
    mOpenGLWidget = mController->GetWidget();

    QQuickWindow::setGraphicsApi(QSGRendererInterface::GraphicsApi::OpenGL);

    mGridLayout = new QGridLayout;
    mBasicSix = new QQuickWidget;
    mBasicSix->rootContext()->setContextProperty("pfd", mPfd);
    mBasicSix->setSource(QUrl("qrc:/Qml/BasicSix.qml"));
    mBasicSix->setAttribute(Qt::WA_TranslucentBackground, true);
    mBasicSix->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    mBasicSix->setClearColor(Qt::transparent);

    mGridLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 0);
    mGridLayout->addWidget(mBasicSix, 1, 1);
    mGridLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 2);
    mOpenGLWidget->setLayout(mGridLayout);

    mBasicSix->installEventFilter(mOpenGLWidget);
    mBasicSix->setVisible(false);
}

void Canavar::Simulator::Simulator::Run()
{
    mController->Run();
    mOpenGLWidget->showMaximized();
}

void Canavar::Simulator::Simulator::Initialize()
{
    initializeOpenGLFunctions();

    mRenderRef = QtImGui::initialize(mOpenGLWidget);

    mNodeManager = mController->GetNodeManager();
    mCameraManager = mController->GetCameraManager();
    mPersecutorCamera = std::make_shared<Canavar::Engine::PersecutorCamera>();

    mAircraft = new Aircraft;
    mAircraftController = new AircraftController(mAircraft);

    // Post initialize
    mNodeManager->ImportNodes("Resources/f16.json");
    mFreeCamera = mCameraManager->GetFreeCamera();
    mRootNode = mNodeManager->FindNodeByName<Canavar::Engine::DummyObject>("Root Node");
    mJetNode = mNodeManager->FindNodeByName<Canavar::Engine::Model>("f16c");
    mDummyCamera = mNodeManager->FindNodeByName<Canavar::Engine::DummyCamera>("Dummy Camera");

    mCameraManager->SetActiveCamera(mPersecutorCamera);
    mPersecutorCamera->SetTarget(mRootNode);
    mPersecutorCamera->SetYaw(180.0f);
    mPersecutorCamera->SetPitch(-10.0f);

    mAircraftController->SetRootNode(mRootNode);
    mAircraftController->SetJetNode(mJetNode);

    mAircraftController->Initialize();
    mAircraft->Initialize();

    connect(mAircraft, &Aircraft::PfdChanged, this, [=](Aircraft::PrimaryFlightData pfd) {
        mPfd->setAirspeed(pfd.airspeed);
        mPfd->setRoll(pfd.roll);
        mPfd->setPitch(pfd.pitch);

        mPfd->setAltitude(pfd.altitude);
        mPfd->setClimbRate(pfd.climbRate / 20.0f);
        mPfd->setHeading(pfd.heading);

        mPfd->setPressure(pfd.pressure);
    });
}

void Canavar::Simulator::Simulator::PostRender(float ifps)
{
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    mAircraftController->Update(ifps);

    QtImGui::newFrame();
    mAircraftController->DrawGui();
    ImGui::Render();
    QtImGui::render();
}

bool Canavar::Simulator::Simulator::KeyPressed(QKeyEvent* pEvent)
{
    mAircraftController->KeyPressed(pEvent);

    if (pEvent->key() == Qt::Key_1)
    {
        mCameraManager->SetActiveCamera(mFreeCamera);
    }
    else if (pEvent->key() == Qt::Key_2)
    {
        mCameraManager->SetActiveCamera(mPersecutorCamera);
    }
    else if (pEvent->key() == Qt::Key_3)
    {
        mCameraManager->SetActiveCamera(mDummyCamera);
        return true;
    }

    return false;
}

bool Canavar::Simulator::Simulator::KeyReleased(QKeyEvent* pEvent)
{
    mAircraftController->KeyReleased(pEvent);

    return false;
}
