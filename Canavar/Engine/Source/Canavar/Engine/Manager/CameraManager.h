#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Object/Camera/FreeCamera.h"

namespace Canavar::Engine
{
    class CameraManager : public Manager
    {
      public:
        explicit CameraManager(QObject *parent = nullptr);

        void Initialize() override;

        void Resize(int w, int h);
        void SetDevicePixelRatio(float dpr);
        void PreUpdate(float ifps) override;

        void OnKeyPressed(QKeyEvent *);
        void OnKeyReleased(QKeyEvent *);
        void OnMousePressed(QMouseEvent *);
        void OnMouseReleased(QMouseEvent *);
        void OnMouseMoved(QMouseEvent *);
        void OnWheelMoved(QWheelEvent *);

        void SetActiveCamera(CameraPtr pCamera);
        CameraPtr GetActiveCamera() const;

      private:
        CameraPtr mActiveCamera{ nullptr };

        DEFINE_MEMBER_CONST(FreeCameraPtr, FreeCamera);
    };

    using CameraManagerPtr = std::shared_ptr<CameraManager>;
}