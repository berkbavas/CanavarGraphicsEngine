#include "LightningStrikeGenerator.h"

#include <QJsonArray>

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

void Canavar::Engine::LightningStrikeGenerator::AddAttractor(LightningStrikeAttractorPtr pAttractor)
{
    mAttractors << pAttractor;
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

void Canavar::Engine::LightningStrikeGenerator::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    LightningStrikeBase::FromJson(object, nodes);

    QJsonArray attractors = object["attractors"].toArray();

    if (attractors.isEmpty())
    {
        return;
    }

    for (const auto &uuid : attractors)
    {
        if (const auto it = nodes.find(uuid.toString()); it != nodes.end())
        {
            if (const auto pAttractor = std::dynamic_pointer_cast<LightningStrikeAttractor>(it->second))
            {
                AddAttractor(pAttractor);
            }
        }
    }
}
