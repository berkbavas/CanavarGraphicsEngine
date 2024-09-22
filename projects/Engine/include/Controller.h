#pragma once

#include "Config.h"

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <QWheelEvent>

namespace Canavar {
    namespace Engine {
        class ModelDataManager;
        class RendererManager;
        class ShaderManager;
        class NodeManager;
        class CameraManager;
        class LightManager;
        class SelectableNodeRenderer;
        class IntersectionManager;
        class Manager;

        class Controller : public QObject, protected QOpenGLExtraFunctions
        {
            Q_OBJECT
        public:
            explicit Controller(QObject* parent = nullptr);

            bool Init(const QString& configFile = "resources/Config/Config.json");
            void MouseDoubleClicked(QMouseEvent* event);
            void MousePressed(QMouseEvent* event);
            void MouseReleased(QMouseEvent* event);
            void MouseMoved(QMouseEvent* event);
            void WheelMoved(QWheelEvent* event);
            void KeyPressed(QKeyEvent* event);
            void KeyReleased(QKeyEvent* event);
            void Resize(int w, int h);
            void Render(float ifps);

            void SetWindow(QOpenGLWindow* newWindow);

        private:
            Config* mConfig;
            ModelDataManager* mModelDataManager;
            RendererManager* mRendererManager;
            ShaderManager* mShaderManager;
            NodeManager* mNodeManager;
            CameraManager* mCameraManager;
            LightManager* mLightManager;
            SelectableNodeRenderer* mSelectableNodeRenderer;
            IntersectionManager* mIntersectionManager;

            QVector<Manager*> mManagers;

            QOpenGLWindow* mWindow;

            bool mSuccess;
        };
    } // namespace Engine
} // namespace Canavar
