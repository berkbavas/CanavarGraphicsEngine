#include "Sun.h"

#include "Canavar/Engine/Node/NodeVisitor.h"

#include <cmath>

Canavar::Engine::Sun::Sun()
{
    SetNodeName("Sun");
    SetDirection(QVector3D(1, -1, 0).normalized());
}

void Canavar::Engine::Sun::Accept(NodeVisitor &visitor)
{
    visitor.Visit(*this);
}
