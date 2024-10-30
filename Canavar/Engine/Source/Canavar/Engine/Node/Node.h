#pragma once

#include "Canavar/Engine/Util/Macros.h"

#include <functional>
#include <map>
#include <memory>

#include <QString>

namespace Canavar::Engine
{
    class Node
    {
      protected:
        Node() = default;
        virtual ~Node() = default;

        DISABLE_COPY(Node);

      public:
        QString GetUniqueNodeName() const;

        DEFINE_MEMBER(QString, NodeName, "Node");
        DEFINE_MEMBER(QString, Uuid);       // Persistent ID
        DEFINE_MEMBER(uint32_t, NodeId, 0); // Run-time ID

        virtual const char* GetNodeType() const = 0;
    };

    using NodePtr = std::shared_ptr<Node>;
}
