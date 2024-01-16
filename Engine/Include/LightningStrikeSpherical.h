#pragma once

#include "LightningStrikeBase.h"

namespace Canavar::Engine
{
    class LightningStrikeSpherical : public LightningStrikeBase
    {
    protected:
        friend class NodeManager;
        LightningStrikeSpherical();

    public:
        int GetNumberOfContactPoints() const;
        void SetNumberOfContactPoints(int NewNumberOfContactPoints);

        QVector<QVector3D> GetWorldPositionsOfTerminationPoints() override;

    private:
        QVector<QVector3D> mContactDirections;
        int mNumberOfContactPoints{ 3 };

        DEFINE_MEMBER(float, Radius);
    };

    using LightningStrikeSphericalWeakPtr = std::weak_ptr<LightningStrikeSpherical>;
    using LightningStrikeSphericalSharedPtr = std::shared_ptr<LightningStrikeSpherical>;
}