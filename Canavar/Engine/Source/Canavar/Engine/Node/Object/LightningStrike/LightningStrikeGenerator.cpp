#include "LightningStrikeGenerator.h"

Canavar::Engine::LightningStrikeGenerator::LightningStrikeGenerator()
{
    SetNodeName("Lightning Strike Generator");
}

QVector<QVector3D> Canavar::Engine::LightningStrikeGenerator::GetWorldPositionsOfTerminationPoints()
{
    QVector<QVector3D> Result;

    for (const auto pAttractor : mAttractors)
    {
        Result << pAttractor->GetWorldPosition();
    }

    return Result;
}

void Canavar::Engine::LightningStrikeGenerator::AddAttractor(LightningStrikeAttractorPtr Attractor)
{
    mAttractors << Attractor;
}
