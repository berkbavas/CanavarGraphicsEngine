#pragma once

#include "Canavar/Engine/Util/Macros.h"
#include "Canavar/Engine/Core/Enums.h"

#include <functional>
#include <map>
#include <memory>

#include <QJsonObject>
#include <QJsonValue>
#include <QSet>
#include <QString>

namespace Canavar::Engine
{
    class Node;
    using NodePtr = std::shared_ptr<Node>;
    using NodeWeakPtr = std::weak_ptr<Node>;

    class Node : public std::enable_shared_from_this<Node>
    {
        DISABLE_COPY(Node);

      public:
        virtual const char* GetNodeTypeName() const = 0;

        QString GetUniqueNodeName() const;

        virtual void ToJson(QJsonObject& jsonObject);
        virtual void FromJson(const QJsonObject& jsonObject, const QSet<NodePtr>& nodes);

        template<class T>
        std::shared_ptr<T> GetParent() const
        {
            return std::dynamic_pointer_cast<T>(mParent.lock());
        }

        virtual void RemoveParent();
        virtual void SetParent(NodeWeakPtr pParentNode);
        virtual void AddChild(NodePtr pNode);
        virtual void RemoveChild(NodePtr pNode);

        const QSet<NodePtr>& GetChildren() const;

      protected:
        Node();
        virtual ~Node() = default;

      private:
        NodeWeakPtr mParent;
        QSet<NodePtr> mChildren;

        DEFINE_MEMBER(QString, NodeName, "Node");
        DEFINE_MEMBER(QString, Uuid, "");   // Persistent ID
        DEFINE_MEMBER(uint32_t, NodeId, 0); // Run-time ID
    };

}
