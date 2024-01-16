#include "LightningStrikeSpherical.h"

#include "Helper.h"

Canavar::Engine::LightningStrikeSpherical::LightningStrikeSpherical()
    : mRadius(1.0f)
{
    mType = Node::NodeType::LightningStrikeSpherical;
    mName = "Lightning Strike Spherical";

    SetNumberOfContactPoints(mNumberOfContactPoints);
}

int Canavar::Engine::LightningStrikeSpherical::GetNumberOfContactPoints() const
{
    return mNumberOfContactPoints;
}

void Canavar::Engine::LightningStrikeSpherical::SetNumberOfContactPoints(int NewNumberOfContactPoints)
{
    mNumberOfContactPoints = NewNumberOfContactPoints;

    mContactDirections.clear();

    while (NewNumberOfContactPoints > 0)
    {
        float x = Helper::GenerateBetween(-10.f, 10.0f);
        float y = Helper::GenerateBetween(-10.f, 10.0f);
        float z = Helper::GenerateBetween(-10.f, 10.0f);
        mContactDirections << QVector3D(x, y, z);
        NewNumberOfContactPoints--;
    }
}

QVector<QVector3D> Canavar::Engine::LightningStrikeSpherical::GetWorldPositionsOfTerminationPoints()
{
    QVector<QVector3D> Result;

    for (const auto& ContactDirection : mContactDirections)
    {
        Result << WorldPosition() + mRadius * ContactDirection;
    }

    return Result;
}
