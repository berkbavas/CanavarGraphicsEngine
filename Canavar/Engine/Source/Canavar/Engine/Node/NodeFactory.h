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
        static NodePtr CreateNode(const QString& type);
        static const QVector<QString>& GetNodeNames();

        static QVector<QString> mNodeNames;
    };
}