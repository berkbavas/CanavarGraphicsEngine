#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

namespace Canavar::Engine
{
    class Text3D : public Object
    {
      public:
        CANAVAR_NODE(Text3D);

        void ToJson(QJsonObject& Object) override;
        void FromJson(const QJsonObject& Object, const QSet<NodePtr>& Nodes) override;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(QString, Text, "");
    };

    using Text3DPtr = std::shared_ptr<Text3D>;
}