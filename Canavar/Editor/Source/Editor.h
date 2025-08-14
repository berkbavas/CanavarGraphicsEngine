#pragma once

#include "Canavar/Engine/Core/Controller.h"

namespace Canavar::Editor
{
    class Editor : public QObject
    {
        Q_OBJECT
      public:
        explicit Editor(QObject *pParent);
        ~Editor();

        void Run();
        void Initialize();

      private:
        Engine::Window *mWindow;
        Engine::Controller *mController;
    };
}