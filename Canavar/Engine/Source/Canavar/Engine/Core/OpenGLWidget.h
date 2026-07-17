#pragma once

#include "Canavar/Engine/Core/RenderingContext.h"

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>

namespace Canavar::Engine
{
    class OpenGLWidget final : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core, public RenderingContext
    {
        Q_OBJECT
      public:
        explicit OpenGLWidget(QWidget* pParent = nullptr);

        void MakeCurrent() override;
        void DoneCurrent() override;
        float GetDevicePixelRatio() const override;

      private:
        void initializeGL() override;
        void resizeGL(int Width, int Height) override;
        void paintGL() override;
        void keyPressEvent(QKeyEvent*) override;
        void keyReleaseEvent(QKeyEvent*) override;
        void mousePressEvent(QMouseEvent*) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void mouseMoveEvent(QMouseEvent*) override;
        void wheelEvent(QWheelEvent*) override;
        void closeEvent(QCloseEvent*) override;
        void leaveEvent(QEvent*) override;

      signals:
        void Initialized();
        void Resized(int Width, int Height);
        void Render(float Ifps);
        void WindowClosed();
        void KeyPressed(QKeyEvent* pEvent);
        void KeyReleased(QKeyEvent* pEvent);
        void MousePressed(QMouseEvent* pEvent);
        void MouseReleased(QMouseEvent* pEvent);
        void MouseMoved(QMouseEvent* pEvent);
        void WheelMoved(QWheelEvent* pEvent);
        void LeaveEvent(QEvent* pEvent);

      private:
        void CheckGLError();

        long long mPreviousTime{ 0 };
        long long mCurrentTime{ 0 };
    };

    using OpenGLWidgetPtr = std::unique_ptr<OpenGLWidget>;
}