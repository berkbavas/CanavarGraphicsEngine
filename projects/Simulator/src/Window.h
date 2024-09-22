#pragma once

#include <FirecrackerEffect.h>
#include <QOpenGLExtraFunctions>
#include <QOpenGLWindow>

#include <imgui.h>
#include <QtImGui.h>

#include "AircraftController.h"

namespace Canavar {
    namespace Engine {
        class Controller;
        class Node;
        class Model;
        class PerspectiveCamera;
        class PersecutorCamera;
        class Gui;
    } // namespace Engine
} // namespace Canavar

class Window : public QOpenGLWindow, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    Window(QWindow* parent = nullptr);

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

    Canavar::Engine::FirecrackerEffect* CreateEfect();

private:
    long long mPreviousTime;
    long long mCurrentTime;
    Canavar::Engine::Controller* mController;

    Aircraft* mAircraft;
    AircraftController* mAircraftController;
    Aircraft::PrimaryFlightData mPfd;

    Canavar::Engine::PerspectiveCamera* mFreeCamera;
    Canavar::Engine::PerspectiveCamera* mDummyCamera;
    Canavar::Engine::PersecutorCamera* mPersecutorCamera;
    Canavar::Engine::Model* mJet;
    Canavar::Engine::Node* mJetRoot;
    Canavar::Engine::Gui* mGui;

    bool mSuccess;
};
