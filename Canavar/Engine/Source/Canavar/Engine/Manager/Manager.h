#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Core/ManagerProvider.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QObject>

namespace Canavar::Engine
{

    class Manager : public QObject, public EventReceiver
    {
      public:
        explicit Manager(QObject* pParent);

        DEFINE_MEMBER_PTR(ManagerProvider, ManagerProvider);
        DEFINE_MEMBER(float, DevicePixelRatio, 1.0f);
    };
}