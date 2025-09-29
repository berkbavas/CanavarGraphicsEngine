#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Object/Camera/FreeCamera.h"

namespace Canavar::Engine
{
    class CameraManager : public Manager
    {
      public:
        using Manager::Manager;

        void Initialize() override;
        void Resize(int Width, int Height) override;
        void Update(float ifps) override;

        void SetDevicePixelRatio(float Ratio);

        bool KeyPressed(QKeyEvent *) override;
        bool KeyReleased(QKeyEvent *) override;
        bool MousePressed(QMouseEvent *) override;
        bool MouseReleased(QMouseEvent *) override;
        bool MouseMoved(QMouseEvent *) override;
        bool WheelMoved(QWheelEvent *) override;

        void SetActiveCamera(CameraPtr pCamera);
        PerspectiveCamera* GetActiveCamera() const;

      private:
        CameraPtr mActiveCamera{ nullptr };

        DEFINE_MEMBER_CONST(FreeCameraPtr, FreeCamera);
    };

    using CameraManagerPtr = std::shared_ptr<CameraManager>;
}