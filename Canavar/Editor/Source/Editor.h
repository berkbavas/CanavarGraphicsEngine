#pragma once

#include "Canavar/Engine/Core/Controller.h"

namespace Canavar::Editor
{
    class Editor : public QObject, public Engine::EventReceiver
    {
        Q_OBJECT
      public:
        explicit Editor(QObject *pParent);
        ~Editor();

        void Run();

      public:
        // Core Events
        void PostInitialize() override;

      private:
        Engine::Window *mWindow;
        Engine::Controller *mController;
    };
}