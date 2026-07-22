#pragma once

#include "Canavar/Engine/Core/Enums.h"
#include "Canavar/Engine/Light/PointLight.h"
#include "Canavar/Engine/Model/PbrMaterial.h"
#include "Canavar/Engine/Model/PhongMaterial.h"
#include "Canavar/Engine/Object/Object.h"

#include <QMap>
#include <QMatrix4x4>

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
        void Render(Scene *pScene, Shader *pShader, RenderPass RenderPass);

        // ── Per-mesh opacity overrides (keyed by MeshId, default 1.0) ────────
        float GetMeshOpacityOverride(int MeshId) const;
        void SetMeshOpacityOverride(int MeshId, float Opacity);
        bool HasMeshOpacityOverride(int MeshId) const;
        const QMap<int, float> &GetMeshOpacityOverrides() const { return mMeshOpacityOverrides; }

        // ── Per-mesh local transform overrides (keyed by MeshId) ─────────────
        const QMatrix4x4 &GetMeshTransformOverride(int MeshId) const;
        void SetMeshTransformOverride(int MeshId, const QMatrix4x4 &Transform);
        bool HasMeshTransformOverride(int MeshId) const;
        const QMap<int, QMatrix4x4> &GetMeshTransformOverrides() const { return mMeshTransformOverrides; }

        DEFINE_MEMBER_CONST(QString, ModelName);
        DEFINE_MEMBER_CONST(std::string, ModelUniqueNameStdString);
        DEFINE_MEMBER(ShadingMode, ShadingMode, ShadingMode::Pbr);
        DEFINE_MEMBER(PhongMaterial, PhongMaterial);
        DEFINE_MEMBER(PbrMaterial, PbrMaterial);
        DEFINE_MEMBER(bool, UseColor, false);
        DEFINE_MEMBER(QVector3D, Color, QVector3D(1, 1, 1));
        DEFINE_MEMBER(float, Opacity, 1.0f);

      private:
        QMap<int, float> mMeshOpacityOverrides;
        QMap<int, QMatrix4x4> mMeshTransformOverrides;

        static const QMatrix4x4 sIdentityMatrix;
    };

    using TexturedModelPtr = std::unique_ptr<TexturedModel>;
}