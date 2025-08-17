#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

namespace Canavar::Engine
{
    class Text3D : public Object
    {
      public:
        CANAVAR_NODE(Text3D);

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes) override;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(QString, Text, "");
    };

    using Text3DPtr = std::shared_ptr<Text3D>;
}