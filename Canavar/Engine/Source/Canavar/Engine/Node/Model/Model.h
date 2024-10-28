#pragma once

#include "Canavar/Engine/Node/Node.h"

namespace Canavar::Engine
{
    class Model : public Node
    {
      public:
        Model(const QString& modelName);

        QMatrix4x4 GetMeshTransformation(const QString& meshName);
        void SetMeshTransformation(const QString& meshName, const QMatrix4x4& transformation);

      private:
        DEFINE_MEMBER(QVector4D, Color, QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Ambient, 0.5f);
        DEFINE_MEMBER(float, Diffuse, 0.5f);
        DEFINE_MEMBER(float, Specular, 0.25f); // TODO: Check if the specular computation is buggy.
        DEFINE_MEMBER(float, Shininess, 8.0f);
        DEFINE_MEMBER_CONST(QString, ModelName);

        QMap<QString, QMatrix4x4> mMeshTransformations;
    };

    using ModelPtr = std::shared_ptr<Model>;
}