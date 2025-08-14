#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Core/RenderingContext.h"
#include "Canavar/Engine/Node/Object/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QObject>

namespace Canavar::Engine
{

    class Manager : public QObject, public EventReceiver
    {
      public:
        explicit Manager(RenderingContext *pRenderingContext, QObject *pParent);

        // Core Events
        // These methods can be overridden by derived classes to implement specific functionality.
        virtual void Initialize();
        virtual void PostInitialize();
        virtual void Resize(int w, int h);
        virtual void Update(float ifps);
        virtual void Render(PerspectiveCamera *pActiveCamera);
        virtual void InRender(PerspectiveCamera *pActiveCamera);
        virtual void PostRender(float ifps);
        virtual void Shutdown();

        DEFINE_MEMBER_PTR_CONST(RenderingContext, RenderingContext);
    };
}