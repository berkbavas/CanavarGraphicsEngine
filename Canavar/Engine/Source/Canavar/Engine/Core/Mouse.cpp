#include "Mouse.h"

void Canavar::Engine::Mouse::SetButtonPressed(Qt::MouseButton Button, bool Pressed)
{
    mButtonPressedStates[Button] = Pressed;
}

bool Canavar::Engine::Mouse::IsButtonPressed(Qt::MouseButton Button) const
{
    return mButtonPressedStates.value(Button, false);
}

void Canavar::Engine::Mouse::SetLastPressPosition(Qt::MouseButton Button, const QPointF &Position)
{
    mButtonLastPressPositions[Button] = Position;
}

QPointF Canavar::Engine::Mouse::GetLastPressPosition(Qt::MouseButton Button) const
{
    return mButtonLastPressPositions.value(Button, QPointF());
}

void Canavar::Engine::Mouse::AddCumulativeMovement(Qt::MouseButton Button, const QPointF &Movement)
{
    mButtonCumulativeMovements[Button] += Movement;
}

void Canavar::Engine::Mouse::ConsumeCumulativeMovement(Qt::MouseButton Button, const QPointF &Movement)
{
    mButtonCumulativeMovements[Button] -= Movement;
}

QPointF Canavar::Engine::Mouse::GetCumulativeMovement(Qt::MouseButton Button) const
{
    return mButtonCumulativeMovements.value(Button, QPointF());
}

void Canavar::Engine::Mouse::ResetCumulativeMovementIfAlmostZero(Qt::MouseButton Button)
{
    const auto Movement = mButtonCumulativeMovements.value(Button, QPointF());

    if (std::abs(Movement.x()) < 0.1f)
    {
        mButtonCumulativeMovements[Button] = QPointF(0, Movement.y());
    }

    if (std::abs(Movement.y()) < 0.1f)
    {
        mButtonCumulativeMovements[Button] = QPointF(Movement.x(), 0);
    }
}

void Canavar::Engine::Mouse::ResetCumulativeMovement(Qt::MouseButton Button)
{
    mButtonCumulativeMovements[Button] = QPointF();
}

void Canavar::Engine::Mouse::Reset()
{
    mButtonPressedStates.clear();
    mButtonLastPressPositions.clear();
    mButtonCumulativeMovements.clear();
}
