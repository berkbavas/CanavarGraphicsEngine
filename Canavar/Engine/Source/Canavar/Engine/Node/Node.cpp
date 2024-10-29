#include "Node.h"

QString Canavar::Engine::Node::GetUniqueNodeName() const
{
    return mNodeName + "##" + QString::number(mNodeId);
}
