#pragma once

#include <QMouseEvent>
#include <QObject>
#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class Window;

    class Controller : public QObject, protected QOpenGLExtraFunctions
    {
        Q_OBJECT
      public:
        explicit Controller(QObject* parent = nullptr);
        ~Controller();

        void Run();

      public slots:
        // Core Events
        void Initialize();
        void Resize(int w, int h);
        void Render(float ifps);

        // Input Events
        void OnKeyPressed(QKeyEvent*);
        void OnKeyReleased(QKeyEvent*);
        void OnMousePressed(QMouseEvent*);
        void OnMouseReleased(QMouseEvent*);
        void OnMouseMoved(QMouseEvent*);
        void OnWheelMoved(QWheelEvent*);

      private:
        Window* mWindow;
    };
}
