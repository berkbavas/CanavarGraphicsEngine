#pragma once

#include "LightningStrikeBase.h"
#include "LightningStrikeAttractor.h"

#include <QSet>

namespace Canavar::Engine
{
    class LightningStrikeGenerator : public LightningStrikeBase
    {
    protected:
        friend class NodeManager;
        LightningStrikeGenerator();
        ~LightningStrikeGenerator() override;

    public:
        QVector<QVector3D> GetWorldPositionsOfTerminationPoints() override;
        void AddAttractor(LightningStrikeAttractor* Attractor);

    private:
        QSet<LightningStrikeAttractor*> mAttractors;
    };
}