#include "NodeFactory.h"

#include "Canavar/Engine/Node/Object/Camera/DummyCamera.h"
#include "Canavar/Engine/Node/Object/DummyObject/DummyObject.h"
#include "Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Light/PointLight.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeAttractor.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"
#include "Canavar/Engine/Node/Object/Text/Text2D.h"
#include "Canavar/Engine/Node/Object/Text/Text3D.h"
#include "Canavar/Engine/Primitive/PrimitiveMesh.h"

#define RETURN_NODE(TYPE) \
    if (type == #TYPE) \
    { \
        return std::make_shared<TYPE>(); \
    }

Canavar::Engine::NodePtr Canavar::Engine::NodeFactory::CreateNode(const std::string &type)
{
    RETURN_NODE(DummyCamera);
    RETURN_NODE(DummyObject);
    RETURN_NODE(NozzleEffect);
    RETURN_NODE(DirectionalLight);
    RETURN_NODE(PointLight);
    RETURN_NODE(LightningStrikeAttractor);
    RETURN_NODE(LightningStrikeGenerator);
    RETURN_NODE(Text2D);
    RETURN_NODE(Text3D);
    RETURN_NODE(Plane);
    RETURN_NODE(Quad);
    RETURN_NODE(Cube);
    RETURN_NODE(Sphere);
    RETURN_NODE(Circle);
    RETURN_NODE(Line);
    RETURN_NODE(Torus);

    return nullptr;
}

const QVector<std::string> &Canavar::Engine::NodeFactory::GetNodeNames()
{
    return mNodeNames;
}

QVector<std::string> Canavar::Engine::NodeFactory::mNodeNames = { "DummyCamera",
                                                                  "DummyObject",
                                                                  "NozzleEffect",
                                                                  "DirectionalLight",
                                                                  "PointLight",
                                                                  "LightningStrikeAttractor", //
                                                                  "LightningStrikeGenerator", //
                                                                  "Text2D",
                                                                  "Text3D",
                                                                  "Plane",
                                                                  "Quad",
                                                                  "Cube",
                                                                  "Sphere",
                                                                  "Circle",
                                                                  "Line",
                                                                  "Torus" };
