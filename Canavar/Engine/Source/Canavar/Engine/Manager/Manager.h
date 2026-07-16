#pragma once

#include "Canavar/Engine/Core/Enums.h"

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
        virtual void Update(float Ifps);
        virtual void Render(RenderPass RenderPass);
        virtual void RenderOverlay(RenderPass RenderPass);
        virtual void DrawImGuiWidget();
        virtual void Resize(int Width, int Height);
        virtual void Paint(QPaintDevice *pPaintDevice);
    };
}