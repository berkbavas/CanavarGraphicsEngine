#pragma once

#include <QInputEvent>
#include <QOpenGLExtraFunctions>
#include <QOpenGLWindow>

namespace Canavar::Engine
{
    class Window : public QOpenGLWindow, public QOpenGLExtraFunctions
    {
        Q_OBJECT
      public:
        Window(QWindow* parent = nullptr);

      private:
        void initializeGL() override;
        void resizeGL(int width, int height) override;
        void paintGL() override;
        void keyPressEvent(QKeyEvent*) override;
        void keyReleaseEvent(QKeyEvent*) override;
        void mousePressEvent(QMouseEvent*) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void mouseMoveEvent(QMouseEvent*) override;
        void wheelEvent(QWheelEvent*) override;

      signals:
        // Core Events
        void Initialize();
        void Resize(int w, int h);
        void Render(float ifps);

        // Input Events
        void KeyPressed(QKeyEvent*);
        void KeyReleased(QKeyEvent*);
        void MousePressed(QMouseEvent*);
        void MouseReleased(QMouseEvent*);
        void MouseMoved(QMouseEvent*);
        void WheelMoved(QWheelEvent*);

      private:
        long long mPreviousTime;
        long long mCurrentTime;
    };
}
