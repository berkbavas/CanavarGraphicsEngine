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

void Canavar::Engine::LightningStrikeGenerator::ToJson(QJsonObject &Object)
{
    LightningStrikeBase::ToJson(Object);

    QJsonArray Attractors;

    for (const auto &pAttractor : mAttractors)
    {
        Attractors.append(QString::fromStdString(pAttractor->GetUuid()));
    }

    Object.insert("attractors", Attractors);
}

void Canavar::Engine::LightningStrikeGenerator::FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes)
{
    LightningStrikeBase::FromJson(Object, Nodes);

    QJsonArray Attractors = Object["attractors"].toArray();

    if (Attractors.isEmpty())
    {
        return;
    }

    for (const auto &uuid : Attractors)
    {
        for (const auto pNode : Nodes)
        {
            if (uuid == QString::fromStdString(pNode->GetUuid()))
            {
                AddAttractor(std::dynamic_pointer_cast<LightningStrikeAttractor>(pNode));
            }
        }
    }
}
