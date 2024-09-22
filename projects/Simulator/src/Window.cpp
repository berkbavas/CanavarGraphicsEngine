#include "Window.h"

#include <CameraManager.h>
#include <Controller.h>
#include <Gui.h>
#include <Helper.h>
#include <Model.h>
#include <Node.h>
#include <NodeManager.h>
#include <PerspectiveCamera.h>
#include <RendererManager.h>

#include <PersecutorCamera.h>
#include <QDateTime>
#include <QKeyEvent>
#include <QVector3D>

#include <QDebug>

using namespace Canavar::Engine;

Window::Window(QWindow* parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
    , mSuccess(false)

{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    format.setSwapInterval(1);
    setFormat(format);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    QtImGui::initialize(this);

    mController = new Canavar::Engine::Controller;
    mController->SetWindow(this);

    mSuccess = mController->Init("resources/Config/Config.json");

    if (mSuccess)
    {
        mJet = dynamic_cast<Canavar::Engine::Model*>(NodeManager::Instance()->GetNodeByName("Model f16c"));
        mJetRoot = NodeManager::Instance()->GetNodeByName("JET_ROOT_NODE");
        mJetRoot->AddChild(mJet);

        mAircraft = new Aircraft;
        mAircraftController = new AircraftController(mAircraft);

        mAircraftController->SetJet(mJet);
        mAircraftController->SetRootJetNode(mJetRoot);

        connect(this, &Window::destroyed, this, [=]() {
            qDebug() << thread() << "Controller is being deleted...";
            mAircraft->Stop();
            });

        connect(mAircraft, &Aircraft::PfdChanged, this, [=](Aircraft::PrimaryFlightData pfd) { mPfd = pfd; }, Qt::QueuedConnection);

        mFreeCamera = dynamic_cast<Canavar::Engine::PerspectiveCamera*>(NodeManager::Instance()->GetNodeByName("Free Camera"));
        mDummyCamera = dynamic_cast<Canavar::Engine::PerspectiveCamera*>(NodeManager::Instance()->GetNodeByName("Dummy Camera"));
        mPersecutorCamera = dynamic_cast<Canavar::Engine::PersecutorCamera*>(NodeManager::Instance()->GetNodeByName("Persecutor Camera"));
        mPersecutorCamera->SetTarget(mJetRoot);

        CameraManager::Instance()->SetActiveCamera(mFreeCamera);
        Canavar::Engine::Sun::Instance()->SetDirection(QVector3D(1, -0.5, 1));

        mGui = new Canavar::Engine::Gui;
        mAircraftController->Init();
        mAircraft->Init();
    }
}

void Window::resizeGL(int w, int h)
{
    glViewport(0, 0, width(), height());

    mController->Resize(w, h);
}

void Window::paintGL()
{
    if (!mSuccess)
        return;

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    mAircraftController->Update(ifps);
    mController->Render(ifps);

    QtImGui::newFrame();

    mGui->Draw();

    mAircraftController->DrawGUI();

    glViewport(0, 0, width(), height());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent* event)
{
    if (!mSuccess)
        return;

    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    if (event->key() == Qt::Key_1)
        CameraManager::Instance()->SetActiveCamera(mFreeCamera);
    else if (event->key() == Qt::Key_2)
        CameraManager::Instance()->SetActiveCamera(mDummyCamera);
    else if (event->key() == Qt::Key_3)
        CameraManager::Instance()->SetActiveCamera(mPersecutorCamera);

    mController->KeyPressed(event);
    mAircraftController->KeyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent* event)
{
    if (!mSuccess)
        return;

    mController->KeyReleased(event);
    mAircraftController->KeyReleased(event);
}

void Window::mousePressEvent(QMouseEvent* event)
{
    if (!mSuccess)
        return;

    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->MousePressed(event);

    mGui->MousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent* event)
{
    if (!mSuccess)
        return;

    mController->MouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent* event)
{
    if (!mSuccess)
        return;

    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->MouseMoved(event);
}

void Window::wheelEvent(QWheelEvent* event)
{
    if (!mSuccess)
        return;

    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->WheelMoved(event);
}

FirecrackerEffect* Window::CreateEfect()
{
    auto fireCracker = dynamic_cast<FirecrackerEffect*>(NodeManager::Instance()->CreateNode(Node::NodeType::FirecrackerEffect));
    fireCracker->SetWorldPosition(QVector3D(Helper::GenerateBetween(0, 1000), Helper::GenerateBetween(0, 1000), Helper::GenerateBetween(0, 1000)));
    fireCracker->SetGravity(20);
    fireCracker->SetSpanAngle(Helper::GenerateBetween(150, 170));
    fireCracker->SetMaxLife(Helper::GenerateBetween(10, 15));
    fireCracker->SetInitialSpeed(Helper::GenerateBetween(200, 300));
    fireCracker->SetScale(1);

    return fireCracker;
}