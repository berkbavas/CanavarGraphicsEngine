#pragma once

#include "Canavar/Engine/Core/Enums.h"
#include "Canavar/Engine/Light/PointLight.h"
#include "Canavar/Engine/Model/PbrMaterial.h"
#include "Canavar/Engine/Model/PhongMaterial.h"
#include "Canavar/Engine/Object/Object.h"

namespace Canavar::Engine
{
    class Scene;
    class Shader;

    class TexturedModel : public Object
    {
      public:
        explicit TexturedModel(const QString &ModelName);

        const char *GetNodeTypeName() const override { return "TexturedModel"; }

        bool IsFuzzyTransparent() const;
        void Render(Scene *pScene, Shader *pShader, RenderPass RenderPass, const QVector<PointLight *> &PointLights);

        DEFINE_MEMBER_CONST(QString, ModelName);
        DEFINE_MEMBER_CONST(std::string, ModelUniqueNameStdString);
        DEFINE_MEMBER(ShadingMode, ShadingMode, ShadingMode::Pbr);
        DEFINE_MEMBER(PhongMaterial, PhongMaterial);
        DEFINE_MEMBER(PbrMaterial, PbrMaterial);
        DEFINE_MEMBER(bool, UseColor, false);
        DEFINE_MEMBER(QVector3D, Color, QVector3D(1, 1, 1));
        DEFINE_MEMBER(float, Opacity, 1.0f);
        DEFINE_MEMBER(bool, Overlay, false);
    };

    using TexturedModelPtr = std::unique_ptr<TexturedModel>;
}