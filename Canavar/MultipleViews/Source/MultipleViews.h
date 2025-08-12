#pragma once

#include "Canavar/Engine/Core/Controller.h"

#include <QHBoxLayout>
#include <QWidget>

namespace Canavar::MultipleViews
{
    class MultipleViews : public QObject, public Engine::EventReceiver
    {
        Q_OBJECT
      public:
        explicit MultipleViews(QObject *pParent);
        ~MultipleViews();

        void Run();

      public:
        // Core Events
        void PostInitialize() override;

      private:
        enum class ViewId
        {
            View0,
            View1
        };
        
        QWidget *mContainerWindow{ nullptr };
        QHBoxLayout *mRootLayout{ nullptr };

        std::map<ViewId, Engine::Widget *> mWidgets;
        std::map<ViewId, Engine::Controller *> mControllers;

        static constexpr std::array<ViewId, 2> VIEW_IDS = { ViewId::View0, ViewId::View1 };
    };
}