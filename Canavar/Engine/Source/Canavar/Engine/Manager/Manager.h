#pragma once

#include "Canavar/Engine/Core/RenderPassParameters.h"

#include <memory>

#include <QObject>
#include <QPaintDevice>

namespace Canavar::Engine
{
    class Manager : public QObject
    {
      public:
        Manager() = default;
        Manager(const Manager &) = delete;
        Manager &operator=(const Manager &) = delete;
        virtual ~Manager() = default;

        virtual void Initialize();
        virtual void PostInitialize();
        virtual void Update(float ifps);
        virtual void Render(const RenderPassParameters &Parameters);
        virtual void DrawImGuiWidget();
        virtual void Resize(int Width, int Height);
        virtual void Paint(QPaintDevice *pPaintDevice);
    };
}