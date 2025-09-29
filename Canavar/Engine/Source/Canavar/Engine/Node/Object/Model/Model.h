#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Node/Object/Object.h"

namespace Canavar::Engine
{

    class Model : public Object
    {
      public:
        CANAVAR_NODE(Model);

        Model(const std::string& SceneName);

        QMatrix4x4 GetMeshTransformation(const std::string& MeshName) const;
        void SetMeshTransformation(const std::string& MeshName, const QMatrix4x4& Transformation);

        bool GetMeshVisibility(const std::string& MeshName) const;
        void SetMeshVisibility(const std::string& MeshName, bool Visible);

        float GetMeshOpacity(const std::string& MeshName, float DefaultOpacity = 1.0f) const;
        void SetMeshOpacity(const std::string& MeshName, float Opacity);

        void ToJson(QJsonObject& Object) override;
        void FromJson(const QJsonObject& Object, const QSet<NodePtr>& Nodes) override;

      private:
        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(QVector3D, TransparencyColor, QVector3D(0.0f, 0.0f, 0.0f)); // For transparency to work we must set the color to black
        DEFINE_MEMBER(float, Ambient, 0.25f);
        DEFINE_MEMBER(float, Diffuse, 0.75f);
        DEFINE_MEMBER(float, Specular, 0.25f);
        DEFINE_MEMBER(float, Shininess, 8.0f);
        DEFINE_MEMBER(float, Metallic, 0.0f);
        DEFINE_MEMBER(float, Roughness, 0.25f);
        DEFINE_MEMBER(float, AmbientOcclusion, 4.0f);
        DEFINE_MEMBER(bool, UseModelColor, false);
        DEFINE_MEMBER(int, ShadingMode, PBR_SHADING);
        DEFINE_MEMBER_CONST(std::string, SceneName);
        DEFINE_MEMBER(int, SelectedMeshId, -1);

        QMap<std::string, QMatrix4x4> mMeshTransformations;
        QMap<std::string, bool> mMeshVisibilities;
        QMap<std::string, float> mMeshOpacities;
    };

    using ModelPtr = std::shared_ptr<Model>;
}