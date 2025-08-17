#pragma once

#include "Canavar/Engine/Node/Node.h"

namespace Canavar::Engine
{
    class NodeVisitor
    {
      public:
        virtual void Visit(class Sun& sun) = 0;
        virtual void Visit(class Sky& sky) = 0;
        virtual void Visit(class Haze& haze) = 0;
        virtual void Visit(class Terrain& terrain) = 0;
        virtual void Visit(class Text2D& text2D) = 0;
        virtual void Visit(class Object& object) = 0;
        virtual void Visit(class Model& model) = 0;
        virtual void Visit(class Text3D& text3D) = 0;
        virtual void Visit(class PerspectiveCamera& camera) = 0;
        virtual void Visit(class DirectionalLight& light) = 0;
        virtual void Visit(class PointLight& light) = 0;
        virtual void Visit(class NozzleEffect& effect) = 0;
        virtual void Visit(class LightningStrikeBase& strike) = 0;
    };
}
