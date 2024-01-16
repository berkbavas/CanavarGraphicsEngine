#include "LightningStrikeGenerator.h"

Canavar::Engine::LightningStrikeGenerator::LightningStrikeGenerator()
    : LightningStrikeBase()
{
    mType = Node::NodeType::LightningStrikeGenerator;
    mName = "Lightning Strike Generator";
}

Canavar::Engine::LightningStrikeGenerator::~LightningStrikeGenerator()
{

}

QVector<QVector3D> Canavar::Engine::LightningStrikeGenerator::GetWorldPositionsOfTerminationPoints()
{
    // TODO: @berkbavas Adopt to the new design

    QVector<QVector3D> Result;

    for (const auto pAttractor : mAttractors)
    {
        Result << pAttractor->WorldPosition();
    }

    return Result;
}

void Canavar::Engine::LightningStrikeGenerator::AddAttractor(LightningStrikeAttractor* Attractor)
{
    mAttractors << Attractor;
}
