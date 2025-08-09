#pragma once

#include "Canavar/Engine/Node/Node.h"

#include <QString>
#include <QVector>

namespace Canavar::Engine
{
    class NodeFactory
    {
      public:
        NodeFactory() = delete;
        static NodePtr CreateNode(const std::string& type);
        static const QVector<std::string>& GetNodeNames();

        static QVector<std::string> mNodeNames;
    };
}