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
    class Node
    {
        DISABLE_COPY(Node);

      protected:
        Node();
        Node(const QString& uuid);
        virtual ~Node() = default;

        virtual void ToJson(QJsonObject& object);
        virtual void FromJson(const QJsonObject& object);

      public:
        QString GetUniqueNodeName() const;

        DEFINE_MEMBER(QString, NodeName, "Node");
        DEFINE_MEMBER_CONST(QString, Uuid); // Persistent ID
        DEFINE_MEMBER(uint32_t, NodeId, 0); // Run-time ID

        virtual const char* GetNodeType() const = 0;
    };

    using NodePtr = std::shared_ptr<Node>;
}
