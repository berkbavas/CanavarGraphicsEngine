#include "LightningStrikeGenerator.h"

#include <QJsonArray>

Canavar::Engine::LightningStrikeGenerator::LightningStrikeGenerator()
{
    SetNodeName("Lightning Strike Generator");
}

void Canavar::Engine::LightningStrikeGenerator::ToJson(QJsonObject &object)
{
    LightningStrikeBase::ToJson(object);

    QJsonArray attractors;

    for (const auto &pAttractor : mAttractors)
    {
        attractors.append(pAttractor->GetUuid());
    }

    object.insert("attractors", attractors);
}

void Canavar::Engine::LightningStrikeGenerator::FromJson(const QJsonObject &object)
{
    LightningStrikeBase::FromJson(object);
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
