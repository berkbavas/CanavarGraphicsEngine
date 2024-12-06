#pragma once

#include <QInputEvent>
#include <QOpenGLWidget>

namespace Canavar::Engine
{
    class CrossSectionAnalyzerWidget : public QOpenGLWidget
    {
        Q_OBJECT
      public:
        explicit CrossSectionAnalyzerWidget(QWidget* parent = nullptr);

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
        void Render();

        // Input Events
        void KeyPressed(QKeyEvent*);
        void KeyReleased(QKeyEvent*);
        void MousePressed(QMouseEvent*);
        void MouseReleased(QMouseEvent*);
        void MouseMoved(QMouseEvent*);
        void WheelMoved(QWheelEvent*);
    };
}
