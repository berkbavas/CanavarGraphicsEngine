#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Camera/FreeCamera.h"

namespace Canavar::Engine
{
    class CameraManager : public Manager
    {
      public:
        explicit CameraManager(QObject *parent = nullptr);

        void Initialize() override;

        void Resize(int w, int h);
        void PreUpdate(float ifps) override;

        void OnKeyPressed(QKeyEvent *);
        void OnKeyReleased(QKeyEvent *);
        void OnMousePressed(QMouseEvent *);
        void OnMouseReleased(QMouseEvent *);
        void OnMouseMoved(QMouseEvent *);
        void OnWheelMoved(QWheelEvent *);

        void SetActiveCamera(CameraPtr camera);
        CameraPtr GetActiveCamera() const;

      private:
        CameraPtr mActiveCamera{ nullptr };
    };

    using CameraManagerPtr = std::shared_ptr<CameraManager>;
}