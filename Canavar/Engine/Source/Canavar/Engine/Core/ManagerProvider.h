#pragma once

#include <memory>

namespace Canavar::Engine
{
    class NodeManager;
    class ShaderManager;
    class CameraManager;
    class RenderingManager;
    class LightManager;

    class ManagerProvider
    {
      public:
        ManagerProvider() = default;
        virtual ~ManagerProvider() = default;
        virtual NodeManager* GetNodeManager() = 0;
        virtual ShaderManager* GetShaderManager() = 0;
        virtual CameraManager* GetCameraManager() = 0;
        virtual RenderingManager* GetRenderingManager() = 0;
        virtual LightManager* GetLightManager() = 0;
    };

    using ManagerProviderPtr = std::shared_ptr<ManagerProvider>;
}