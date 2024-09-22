#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLWindow>

#include <imgui.h>
#include <QtImGui.h>

namespace Canavar {
    namespace Engine {
        class Controller;
        class PerspectiveCamera;
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

private:
    long long mPreviousTime;
    long long mCurrentTime;
    Canavar::Engine::Controller* mController;
    Canavar::Engine::Gui* mGui;
    Canavar::Engine::PerspectiveCamera* mCamera;
};
#endif // WINDOW_H
