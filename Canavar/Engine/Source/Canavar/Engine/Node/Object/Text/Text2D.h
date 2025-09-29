#pragma once

#include "Canavar/Engine/Node/Node.h"

#include <QString>
#include <QVector3D>

namespace Canavar::Engine
{
    class Text2D : public Node
    {
      public:
        CANAVAR_NODE(Text2D);

        void ToJson(QJsonObject &Object) override;
        void FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes) override;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(QVector2D, Position, QVector2D(0.0f, 0.0f));
        DEFINE_MEMBER(float, Scale, 1.0f);
        DEFINE_MEMBER(QString, Text, "");
    };

    using Text2DPtr = std::shared_ptr<Text2D>;
}