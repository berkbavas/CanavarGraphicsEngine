#pragma once

#include "Canavar/Engine/Core/Enums.h"
#include "Canavar/Engine/Node/NodeVisitor.h"
#include "Canavar/Engine/Util/Macros.h"

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
        virtual void Accept(NodeVisitor& visitor) = 0;

        const std::string& GetUniqueNodeName();
        const std::string& GetNodeIdString();

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
        virtual void RemoveChild(Node* pNode);

        const QSet<NodePtr>& GetChildren() const;

      protected:
        Node();
        virtual ~Node() = default;

      private:
        NodeWeakPtr mParent;
        QSet<NodePtr> mChildren;
        std::string mUniqueNodeName; // Unique name for this node, used for identification.
        std::string mNodeIdString;

        DEFINE_MEMBER(std::string, NodeName, "Node");
        DEFINE_MEMBER(std::string, Uuid, ""); // Persistent ID
        DEFINE_MEMBER(int, NodeId, 0);        // Run-time ID
    };

}
