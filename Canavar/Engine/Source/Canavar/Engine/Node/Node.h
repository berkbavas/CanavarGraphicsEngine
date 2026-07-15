#pragma once

#include "Canavar/Engine/Core/Constants.h"

#include <format>
#include <memory>
#include <string>

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

      private:
        void UpdateNodeUniqueName();
        void SetUuid(const QUuid& Uuid);
        void SetNodeId(int NodeId);

        std::string mNodeName;            // Human-readable name of the node
        std::string mNodeUniqueName;      // Unique name generated from NodeName and NodeId
        QUuid mUuid;                      // Persistent ID
        int mNodeId{ UNDEFINED_NODE_ID }; // Runtime ID

        friend class NodeManager;
    };

    using NodePtr = std::unique_ptr<Node>;

}