#pragma once

#include "Canavar/Engine/Core/ManagerProvider.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QObject>

namespace Canavar::Engine
{

    class Manager : public QObject
    {
      public:
        explicit Manager(QObject* pParent);

        virtual void Initialize() = 0;
        virtual void PostInitialize();
        virtual void Shutdown();

        virtual void Update(float ifps);
        virtual void Render(float ifps);

        DEFINE_MEMBER_PTR(ManagerProvider, ManagerProvider);
        DEFINE_MEMBER(float, DevicePixelRatio, 1.0f);
    };
}