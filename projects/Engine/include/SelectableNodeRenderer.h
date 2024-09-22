#pragma once

#include "Manager.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLVertexArrayObject.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
    namespace Engine {
        class ShaderManager;
        class ModelDataManager;
        class NodeManager;
        class CameraManager;
        class RendererManager;
        class Config;

        class SelectableNodeRenderer : public Manager, protected QOpenGLExtraFunctions
        {
        private:
            SelectableNodeRenderer();

        public:
            struct NodeInfo {
                int nodeID;
                int meshID;
                int vertexID;
                int success;
            };

            static SelectableNodeRenderer* Instance();

            bool Init() override;
            void Render(float ifps) override;
            void Resize(int width, int height) override;

            NodeInfo GetNodeInfoByScreenPosition(int x, int y);
            NodeInfo GetVertexInfoByScreenPosition(int x, int y);

        private:
            Config* mConfig;
            ShaderManager* mShaderManager;
            NodeManager* mNodeManager;
            CameraManager* mCameraManager;
            RendererManager* mRendererManager;

            OpenGLVertexArrayObject mCube;

            OpenGLFramebuffer mNodeInfoFBO;
            int mWidth;
            int mHeight;
            bool mResizeLater;
        };
    } // namespace Engine
} // namespace Canavar
