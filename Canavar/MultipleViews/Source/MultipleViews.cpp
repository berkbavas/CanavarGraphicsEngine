#include "MultipleViews.h"

#include "Canavar/Engine/Core/Widget.h"
#include "Canavar/Engine/Util/Logger.h"

Canavar::MultipleViews::MultipleViews::MultipleViews(QObject *pParent)
    : QObject(pParent)
{
    mContainerWindow = new QWidget(nullptr);
    mRootLayout = new QHBoxLayout(mContainerWindow);
    mContainerWindow->setLayout(mRootLayout);

    for (const auto viewId : VIEW_IDS)
    {
        const auto pWidget = new Engine::Widget(mContainerWindow);
        mWidgets[viewId] = pWidget;
        mControllers[viewId] = new Engine::Controller(pWidget, true, this);
        mRootLayout->addWidget(pWidget);
        pWidget->setFocusPolicy(Qt::StrongFocus);
    }

    mContainerWindow->resize(800, 600);
}

Canavar::MultipleViews::MultipleViews::~MultipleViews()
{
    LOG_DEBUG("MultipleViews::~MultipleViews: Destructor called");
}

void Canavar::MultipleViews::MultipleViews::Run()
{
    mContainerWindow->showMaximized();
}

void Canavar::MultipleViews::MultipleViews::PostInitialize()
{
    mControllers[ViewId::View0]->GetNodeManager()->ImportNodes("Resources/Empty.json");
    mControllers[ViewId::View1]->GetNodeManager()->ImportNodes("Resources/Empty.json");
}
