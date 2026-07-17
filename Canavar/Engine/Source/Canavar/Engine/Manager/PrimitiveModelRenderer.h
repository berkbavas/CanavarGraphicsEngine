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
        void RenderOverlay(RenderPass RenderPass) override;

      private:
        void RenderPrimitiveModels(RenderPass RenderPass, bool OverlayPass, PerspectiveCamera *pCamera);
        bool ShouldRenderPrimitiveModel(PrimitiveModel *pPrimitiveModel, RenderPass RenderPass, bool OverlayPass) const;
        void SetCommonUniforms(PerspectiveCamera *pCamera);

        Renderer *mRenderer{ nullptr };
        NodeManager *mNodeManager{ nullptr };

        std::map<PrimitiveType, GeometryBasePtr> mGeometries;
        std::map<PrimitiveType, Shader *> mShaders;

        ShaderPtr mPrimitiveShader{ nullptr };  // Disk, Plane, Sphere
        ShaderPtr mCircleLineShader{ nullptr }; // Circle, Line
    };

    using PrimitiveModelRendererPtr = std::unique_ptr<PrimitiveModelRenderer>;

} // namespace Canavar::Engine
