#pragma once

#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class Controller;
    class NodeManager;
    class ShaderManager;
    class CameraManager;
    class RenderingManager;
    class LightManager;
    class Painter;
    class TextRenderer;
    class BoundingBoxRenderer;
    class RenderingContext;
    class ShadowMappingRenderer;

    class RenderingContext
    {
      public:
        RenderingContext() = default;
        virtual ~RenderingContext() = default;

        virtual void MakeCurrent() = 0;
        virtual void DoneCurrent() = 0;
        virtual float GetDevicePixelRatio() const = 0;

        DEFINE_MEMBER_PTR_CONST(NodeManager, NodeManager);
        DEFINE_MEMBER_PTR_CONST(ShaderManager, ShaderManager);
        DEFINE_MEMBER_PTR_CONST(CameraManager, CameraManager);
        DEFINE_MEMBER_PTR_CONST(RenderingManager, RenderingManager);
        DEFINE_MEMBER_PTR_CONST(LightManager, LightManager);
        DEFINE_MEMBER_PTR_CONST(Painter, Painter);
        DEFINE_MEMBER_PTR_CONST(TextRenderer, TextRenderer);
        DEFINE_MEMBER_PTR_CONST(BoundingBoxRenderer, BoundingBoxRenderer);
        DEFINE_MEMBER_PTR_CONST(ShadowMappingRenderer, ShadowMappingRenderer);

        friend class Controller;
    };
}