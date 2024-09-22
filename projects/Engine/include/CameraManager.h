#pragma once

#include "Camera.h"
#include "FreeCamera.h"
#include "Manager.h"
#include "NodeManager.h"

#include <QObject>

namespace Canavar {
    namespace Engine {
        class CameraManager : public Manager
        {
            Q_OBJECT

        private:
            friend class Canavar::Engine::NodeManager;

            CameraManager();

            void AddCamera(Camera* camera);
            void RemoveCamera(Camera* camera);

        public:
            static CameraManager* Instance();

            bool Init() override;
            void PostInit() override;
            void MouseDoubleClicked(QMouseEvent* event) override;
            void WheelMoved(QWheelEvent* event) override;
            void MousePressed(QMouseEvent* event) override;
            void MouseReleased(QMouseEvent* event) override;
            void MouseMoved(QMouseEvent* event) override;
            void KeyPressed(QKeyEvent* event) override;
            void KeyReleased(QKeyEvent* event) override;
            void Resize(int width, int height) override;
            void Update(float ifps) override;
            void Reset() override;

            Camera* GetActiveCamera() const;
            void SetActiveCamera(Camera* newActiveCamera);

        private:
            FreeCamera* mDefaultCamera;
            Camera* mActiveCamera;
            QList<Camera*> mCameras;

            int mWidth;
            int mHeight;
        };
    } // namespace Engine
} // namespace Canavar
