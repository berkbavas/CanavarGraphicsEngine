#include "NodeFactory.h"

#include "Canavar/Engine/Node/GlobalNode/Haze/Haze.h"
#include "Canavar/Engine/Node/Object/Camera/DummyCamera.h"
#include "Canavar/Engine/Node/Object/DummyObject/DummyObject.h"
#include "Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Light/PointLight.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeAttractor.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"

#define RETURN_NODE(TYPE) \
    if (type == #TYPE) \
    { \
        return std::make_shared<TYPE>(); \
    }

Canavar::Engine::NodePtr Canavar::Engine::NodeFactory::CreateNode(const QString &type)
{
    RETURN_NODE(Haze);
    RETURN_NODE(DummyCamera);
    RETURN_NODE(DummyObject);
    RETURN_NODE(NozzleEffect);
    RETURN_NODE(DirectionalLight);
    RETURN_NODE(PointLight);
    RETURN_NODE(LightningStrikeAttractor);
    RETURN_NODE(LightningStrikeGenerator);

    return nullptr;
}

const QVector<QString> &Canavar::Engine::NodeFactory::GetNodeNames()
{
    return mNodeNames;
}

QVector<QString> Canavar::Engine::NodeFactory::mNodeNames = { "Haze",
                                                              "DummyCamera",
                                                              "DummyObject",
                                                              "NozzleEffect",
                                                              "DirectionalLight",
                                                              "PointLight",
                                                              "LightningStrikeAttractor", //
                                                              "LightningStrikeGenerator" };
