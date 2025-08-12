#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Object/Camera/FreeCamera.h"

namespace Canavar::Engine
{
    class CameraManager : public Manager, public EventReceiver
    {
      public:
        explicit CameraManager(QObject *pParent);

        void Initialize() override;

        void Resize(int w, int h);
        void SetDevicePixelRatio(float dpr);
        void Update(float ifps) override;

        bool KeyPressed(QKeyEvent *) override;
        bool KeyReleased(QKeyEvent *) override;
        bool MousePressed(QMouseEvent *) override;
        bool MouseReleased(QMouseEvent *) override;
        bool MouseMoved(QMouseEvent *) override;
        bool WheelMoved(QWheelEvent *) override;

        void SetActiveCamera(CameraPtr pCamera);
        CameraPtr GetActiveCamera() const;

      private:
        CameraPtr mActiveCamera{ nullptr };

        DEFINE_MEMBER_CONST(FreeCameraPtr, FreeCamera);
    };

    using CameraManagerPtr = std::shared_ptr<CameraManager>;
}