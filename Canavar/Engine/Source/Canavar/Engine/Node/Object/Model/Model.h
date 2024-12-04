#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

namespace Canavar::Engine
{
    class Model : public Object
    {
      public:
        Model(const QString& sceneName);

        QMatrix4x4 GetMeshTransformation(const QString& meshName);
        void SetMeshTransformation(const QString& meshName, const QMatrix4x4& transformation);

        QString GetNodeType() const override { return "Model"; }
        static constexpr const char* NODE_TYPE{ "Model" };

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes) override;

      private:
        DEFINE_MEMBER(QVector4D, Color, QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Ambient, 0.25f);
        DEFINE_MEMBER(float, Diffuse, 0.75f);
        DEFINE_MEMBER(float, Specular, 0.25f);
        DEFINE_MEMBER(float, Shininess, 8.0f);
        DEFINE_MEMBER_CONST(QString, SceneName);

        DEFINE_MEMBER(bool, InvertNormals, false);

        QMap<QString, QMatrix4x4> mMeshTransformations;
    };

    using ModelPtr = std::shared_ptr<Model>;
}