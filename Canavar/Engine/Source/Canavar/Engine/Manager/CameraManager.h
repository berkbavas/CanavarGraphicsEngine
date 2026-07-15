#pragma once

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Manager/Manager.h"

namespace Canavar::Engine
{
    class CameraManager : public Manager, public EventReceiver
    {
      public:
        CameraManager() = default;

        PerspectiveCamera* GetActiveCamera() const;
        void SetActiveCamera(PerspectiveCamera* pCamera);

        void Update(float Ifps) override;
        void Resize(int Width, int Height) override;
        bool OnKeyPressed(QKeyEvent* pEvent) override;
        bool OnKeyReleased(QKeyEvent* pEvent) override;
        bool OnMousePressed(QMouseEvent* pEvent) override;
        bool OnMouseReleased(QMouseEvent* pEvent) override;
        bool OnMouseMoved(QMouseEvent* pEvent) override;
        bool OnWheelMoved(QWheelEvent* pEvent) override;
        bool OnLeaveEvent(QEvent* pEvent) override;

      private:
        PerspectiveCamera* mActiveCamera{ nullptr };
    };

    using CameraManagerPtr = std::unique_ptr<CameraManager>;
}