#pragma once

#include "Core/ManagerProvider.h"
#include "Util/Macros.h"

#include <QObject>

namespace Canavar::Engine
{

    class Manager : public QObject
    {
      public:
        explicit Manager(QObject* parent = nullptr);

        virtual void Initialize() = 0;
        virtual void PostInitialize();

        virtual void PreUpdate(float ifps);
        virtual void Update(float ifps);
        virtual void PostUpdate(float ifps);

        virtual void Render(float ifps);

        DEFINE_MEMBER_PTR(ManagerProvider, ManagerProvider);
    };
}