#include "DualView.h"

#include <QHBoxLayout>

Canavar::DualView::DualView::DualView()
{
    mContainer = new QWidget(nullptr);

    mLeftWidget = new Canavar::Engine::OpenGLWidget(mContainer);
    mRightWidget = new Canavar::Engine::OpenGLWidget(mContainer);

    QHBoxLayout* pLayout = new QHBoxLayout(mContainer);
    pLayout->addWidget(mLeftWidget);
    pLayout->addWidget(mRightWidget);

    mLeftRenderer = std::make_unique<Canavar::Engine::Renderer>(mLeftWidget);
    mRightRenderer = std::make_unique<Canavar::Engine::Renderer>(mRightWidget);

    mLeftImGui = std::make_unique<Canavar::Engine::ImGuiWidget>(mLeftRenderer.get());
    mRightImGui = std::make_unique<Canavar::Engine::ImGuiWidget>(mRightRenderer.get());
}

Canavar::DualView::DualView::~DualView()
{
    mLeftWidget->MakeCurrent();
    mLeftImGui = nullptr;
    mLeftRenderer = nullptr;

    mRightWidget->MakeCurrent();
    mRightImGui = nullptr;
    mRightRenderer = nullptr;
}

void Canavar::DualView::DualView::Run()
{
    mContainer->resize(1600, 900);
    mContainer->showMinimized();
    mContainer->showMaximized();
}
