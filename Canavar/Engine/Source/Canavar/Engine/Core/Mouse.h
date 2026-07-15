#pragma once

#include <QMap>
#include <QMouseEvent>
#include <QObject>
#include <QPointF>

namespace Canavar::Engine
{
    class Mouse
    {
      public:
        Mouse() = default;

        void SetButtonPressed(Qt::MouseButton Button, bool Pressed);
        bool IsButtonPressed(Qt::MouseButton Button) const;

        void SetLastPressPosition(Qt::MouseButton Button, const QPointF& Position);
        QPointF GetLastPressPosition(Qt::MouseButton Button) const;

        void AddCumulativeMovement(Qt::MouseButton Button, const QPointF& Movement);
        void ConsumeCumulativeMovement(Qt::MouseButton Button, const QPointF& Movement);
        QPointF GetCumulativeMovement(Qt::MouseButton Button) const;

        void ResetCumulativeMovementIfAlmostZero(Qt::MouseButton Button);
        void ResetCumulativeMovement(Qt::MouseButton Button);
        void Reset();

      private:
        QMap<Qt::MouseButton, bool> mButtonPressedStates;
        QMap<Qt::MouseButton, QPointF> mButtonLastPressPositions;
        QMap<Qt::MouseButton, QPointF> mButtonCumulativeMovements;
    };
}