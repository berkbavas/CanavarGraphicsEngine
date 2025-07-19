#pragma once

#include "Canavar/Engine/Util/Macros.h"

#include <functional>
#include <map>
#include <memory>

#include <QJsonObject>
#include <QJsonValue>
#include <QString>

namespace Canavar::Engine
{
    class Node;
    using NodePtr = std::shared_ptr<Node>;

    class Node
    {
        DISABLE_COPY(Node);

      protected:
        Node();
        virtual ~Node() = default;

      public:
        virtual void ToJson(QJsonObject& object);
        virtual void FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes);

        QString GetUniqueNodeName() const;

        DEFINE_MEMBER(QString, NodeName, "Node");
        DEFINE_MEMBER(QString, Uuid);       // Persistent ID
        DEFINE_MEMBER(uint32_t, NodeId, 0); // Run-time ID

        virtual const QString& GetNodeType() const = 0;
    };

}
