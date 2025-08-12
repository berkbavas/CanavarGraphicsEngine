#include "Editor.h"

#include "Canavar/Engine/Core/Window.h"
#include "Canavar/Engine/Util/Logger.h"

Canavar::Editor::Editor::Editor(QObject *pParent)
    : QObject(pParent)
{
    mWindow = new Canavar::Engine::Window(nullptr);
    mController = new Canavar::Engine::Controller(mWindow, true, this);
    mController->AddEventReceiver(this);
}

Canavar::Editor::Editor::~Editor()
{
    LOG_DEBUG("Editor::~Editor: Destructor called");
}

void Canavar::Editor::Editor::Run()
{
    mWindow->showMinimized();
}

void Canavar::Editor::Editor::PostInitialize()
{
    mWindow->showMaximized();
    mController->GetNodeManager()->ImportNodes("Resources/Empty.json");
}
