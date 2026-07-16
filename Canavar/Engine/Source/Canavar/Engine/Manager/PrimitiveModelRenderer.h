#pragma once

#include "Canavar/Engine/Core/Enums.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

#include <map>
#include <memory>

namespace Canavar::Engine
{
    class Renderer;
    class NodeManager;
    class PrimitiveModel;
    class PerspectiveCamera;

    class PrimitiveModelRenderer : public Manager
    {
      public:
        explicit PrimitiveModelRenderer(Renderer *pRenderer);
        virtual ~PrimitiveModelRenderer() = default;

        void Initialize() override;
        void Render(RenderPass RenderPass) override;
        void Resize(int Width, int Height) override;

      private:
        // Render Disk / Plane / Sphere using the Primitive shader
        void RenderSolidPrimitives(RenderPass RenderPass, PerspectiveCamera *pCamera);

        // Render Circle / Line using the CircleLine shader (geometry stage)
        void RenderLinePrimitives(PerspectiveCamera *pCamera);

        void SetSolidCommonUniforms(PerspectiveCamera *pCamera);
        void SetLineCommonUniforms(PerspectiveCamera *pCamera);

        Renderer *mRenderer{ nullptr };
        NodeManager *mNodeManager{ nullptr };

        std::map<PrimitiveType, GeometryBasePtr> mGeometries;

        ShaderPtr mPrimitiveShader{ nullptr };  // Disk, Plane, Sphere
        ShaderPtr mCircleLineShader{ nullptr }; // Circle, Line

        int mWidth{ 1 };
        int mHeight{ 1 };
    };

    using PrimitiveModelRendererPtr = std::unique_ptr<PrimitiveModelRenderer>;

} // namespace Canavar::Engine
