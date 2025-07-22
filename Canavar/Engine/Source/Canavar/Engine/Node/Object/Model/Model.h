#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Node/Object/Object.h"

namespace Canavar::Engine
{

    class Model : public Object
    {
        REGISTER_NODE_TYPE(Model);

      public:
        Model(const QString& sceneName);

        QMatrix4x4 GetMeshTransformation(const QString& meshName);
        void SetMeshTransformation(const QString& meshName, const QMatrix4x4& transformation);

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes) override;

      private:
        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Ambient, 0.25f);
        DEFINE_MEMBER(float, Diffuse, 0.75f);
        DEFINE_MEMBER(float, Specular, 0.25f);
        DEFINE_MEMBER(float, Shininess, 8.0f);
        DEFINE_MEMBER(float, Metallic, 0.0f);
        DEFINE_MEMBER(float, Roughness, 0.0f);
        DEFINE_MEMBER(float, AmbientOcclusion, 3.0f);
        DEFINE_MEMBER(int, ShadingMode, PBR_SHADING);
        DEFINE_MEMBER(bool, UseColor, false);
        DEFINE_MEMBER_CONST(QString, SceneName);

        DEFINE_MEMBER(bool, InvertNormals, false);

        QMap<QString, QMatrix4x4> mMeshTransformations;
    };

    using ModelPtr = std::shared_ptr<Model>;
}