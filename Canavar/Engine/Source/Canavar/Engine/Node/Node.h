#pragma once

#include "Canavar/Engine/Core/Constants.h"

#include <format>
#include <memory>
#include <string>
#include <vector>

#include <QUuid>
#include <QVector3D>

namespace Canavar::Engine
{
    class Node
    {
      public:
        Node() = default;
        virtual ~Node() = default;

        virtual const char* GetNodeTypeName() const = 0;

        const QUuid& GetUuid() const;
        const std::string& GetNodeName() const;
        const std::string& GetNodeUniqueName() const;
        int GetNodeId() const;

        void SetNodeName(const std::string& NodeName);

        // ── Hierarchy ────────────────────────────────────────────────────────
        Node* GetParent() const;
        const std::vector<Node*>& GetChildren() const;

        // Attaches pChild under this node (detaches from its current parent first).
        void AddChild(Node* pChild);

        // Detaches pChild from this node. No-op if pChild is not a direct child.
        void RemoveChild(Node* pChild);

        // Convenience: reparents this node. Pass nullptr to detach from any parent.
        void SetParent(Node* pParent);

      private:
        void UpdateNodeUniqueName();
        void SetUuid(const QUuid& Uuid);
        void SetNodeId(int NodeId);

        std::string mNodeName;            // Human-readable name of the node
        std::string mNodeUniqueName;      // Unique name generated from NodeName and NodeId
        QUuid mUuid;                      // Persistent ID
        int mNodeId{ UNDEFINED_NODE_ID }; // Runtime ID

        Node* mParent{ nullptr };
        std::vector<Node*> mChildren;

        friend class NodeManager;
    };

    using NodePtr = std::unique_ptr<Node>;

}