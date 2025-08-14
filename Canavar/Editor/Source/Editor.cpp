#include "Editor.h"

#include "Canavar/Engine/Core/Window.h"
#include "Canavar/Engine/Util/Logger.h"

Canavar::Editor::Editor::Editor(QObject *pParent)
    : QObject(pParent)
{
    mWindow = new Canavar::Engine::Window(nullptr);
    mController = new Canavar::Engine::Controller(mWindow, true, this);
    connect(mController, &Canavar::Engine::Controller::Initialized, this, &Editor::Initialize);
}

Canavar::Editor::Editor::~Editor()
{
    LOG_DEBUG("Editor::~Editor: Destructor called");
}

void Canavar::Editor::Editor::Run()
{
    mWindow->showMinimized();
}

void Canavar::Editor::Editor::Initialize()
{
    mWindow->showMaximized();
    mWindow->GetNodeManager()->ImportNodes("Resources/Empty.json");
}
