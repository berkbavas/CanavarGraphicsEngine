#pragma once

#include "Canavar/Engine/Core/RenderingContext.h"

#include <QInputEvent>
#include <QOpenGLWidget>

namespace Canavar::Engine
{
    class Widget : public QOpenGLWidget, public RenderingContext
    {
        Q_OBJECT

      public:
        explicit Widget(QWidget *pParent = nullptr);

        void MakeCurrent() override;
        void DoneCurrent() override;
        float GetDevicePixelRatio() const override;

      signals:
        // Core Events
        void Initialize();
        void Resize(int Width, int Height);
        void Render(float ifps);

        // Input Events
        void KeyPressed(QKeyEvent *);
        void KeyReleased(QKeyEvent *);
        void MousePressed(QMouseEvent *);
        void MouseReleased(QMouseEvent *);
        void MouseMoved(QMouseEvent *);
        void WheelMoved(QWheelEvent *);

      private:
        void initializeGL() override;
        void resizeGL(int Width, int Height) override;
        void paintGL() override;
        void keyPressEvent(QKeyEvent *) override;
        void keyReleaseEvent(QKeyEvent *) override;
        void mousePressEvent(QMouseEvent *) override;
        void mouseReleaseEvent(QMouseEvent *) override;
        void mouseMoveEvent(QMouseEvent *) override;
        void wheelEvent(QWheelEvent *) override;

        bool eventFilter(QObject *pReceiver, QEvent *pEvent) override;

      private:
        long long mPreviousTime;
        long long mCurrentTime;
    };
}
