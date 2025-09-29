#pragma once

#include "Canavar/Engine/Node/Node.h"

namespace Canavar::Engine
{
    class NodeVisitor
    {
      public:
        virtual void Visit(class Sun& Sun) = 0;
        virtual void Visit(class Sky& Sky) = 0;
        virtual void Visit(class Haze& Haze) = 0;
        virtual void Visit(class Terrain& Terrain) = 0;
        virtual void Visit(class Text2D& Text2D) = 0;
        virtual void Visit(class Object& Object) = 0;
        virtual void Visit(class Model& Model) = 0;
        virtual void Visit(class Text3D& Text3D) = 0;
        virtual void Visit(class PerspectiveCamera& Camera) = 0;
        virtual void Visit(class DirectionalLight& Light) = 0;
        virtual void Visit(class PointLight& Light) = 0;
        virtual void Visit(class NozzleEffect& Effect) = 0;
        virtual void Visit(class LightningStrikeBase& LightningStrikeBase) = 0;
    };
}
