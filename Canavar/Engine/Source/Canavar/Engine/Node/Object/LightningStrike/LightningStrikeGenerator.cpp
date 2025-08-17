#include "LightningStrikeGenerator.h"

#include <QJsonArray>

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
        attractors.append(QString::fromStdString(pAttractor->GetUuid()));
    }

    object.insert("attractors", attractors);
}

void Canavar::Engine::LightningStrikeGenerator::FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes)
{
    LightningStrikeBase::FromJson(object, nodes);

    QJsonArray attractors = object["attractors"].toArray();

    if (attractors.isEmpty())
    {
        return;
    }

    for (const auto &uuid : attractors)
    {
        for (const auto pNode : nodes)
        {
            if (uuid == QString::fromStdString(pNode->GetUuid()))
            {
                AddAttractor(std::dynamic_pointer_cast<LightningStrikeAttractor>(pNode));
            }
        }
    }
}
