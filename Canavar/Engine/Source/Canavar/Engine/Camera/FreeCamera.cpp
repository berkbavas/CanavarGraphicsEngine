#include "FreeCamera.h"

#include <QtMath>

Canavar::Engine::FreeCamera::FreeCamera()
{
    SetNodeName("Free Camera");
}

void Canavar::Engine::FreeCamera::Update(float Ifps)
{
    // Rotation
    if (mMouse.IsButtonPressed(Qt::MiddleButton))
    {
        // Calculate delta yaw and pitch based on mouse movement and angular speed
        const auto AngularSpeed = CalculateAngularSpeed(Ifps);
        const auto YawDelta = AngularSpeed * mMouse.GetCumulativeMovement(Qt::MiddleButton).x();
        const auto PitchDelta = AngularSpeed * mMouse.GetCumulativeMovement(Qt::MiddleButton).y();

        mYaw -= YawDelta;
        mPitch -= PitchDelta;

        // Update rotation based on yaw and pitch
        SetRotation(QQuaternion::fromAxisAndAngle(POSITIVE_Y, mYaw) * QQuaternion::fromAxisAndAngle(POSITIVE_X, mPitch));
        mMouse.ResetCumulativeMovement(Qt::MiddleButton);
    }

    // Translation
    const auto& PressedKeys = mPressedKeys.toStdMap(); // Convert QMap to std::map for structured binding

    for (const auto& [Key, IsPressed] : PressedKeys)
    {
        if (IsPressed)
        {
            const auto& Rotation = GetRotation();
            const auto LinearSpeed = CalculateLinearSpeed(Ifps);
            const auto Direction = KEY_BINDINGS.value(Key, ZERO_VECTOR_3D);
            const auto RotatedVector = Rotation.rotatedVector(Direction);
            const auto TranslationDelta = LinearSpeed * RotatedVector;
            Translate(TranslationDelta);
        }
    }
}

void Canavar::Engine::FreeCamera::Reset()
{
    mMouse.Reset();
    mPressedKeys.clear();
}

bool Canavar::Engine::FreeCamera::OnKeyPressed(QKeyEvent* pEvent)
{
    mPressedKeys[(Qt::Key) pEvent->key()] = true;
    return true; // Consume the event to prevent further propagation
}

bool Canavar::Engine::FreeCamera::OnKeyReleased(QKeyEvent* pEvent)
{
    mPressedKeys[(Qt::Key) pEvent->key()] = false;
    return false; // Let the event propagate
}

bool Canavar::Engine::FreeCamera::OnMousePressed(QMouseEvent* pEvent)
{
    if (pEvent->button() == Qt::MiddleButton)
    {
        mMouse.SetButtonPressed(Qt::MiddleButton, true);
        mMouse.SetLastPressPosition(Qt::MiddleButton, pEvent->position());
        return true; // Consume the event to prevent further propagation
    }

    return false; // Let the event propagate
}

bool Canavar::Engine::FreeCamera::OnMouseReleased(QMouseEvent* pEvent)
{
    if (pEvent->button() == Qt::MiddleButton)
    {
        mMouse.SetButtonPressed(Qt::MiddleButton, false);
        return true; // Consume the event to prevent further propagation
    }

    return false; // Let the event propagate
}

bool Canavar::Engine::FreeCamera::OnMouseMoved(QMouseEvent* pEvent)
{
    // We are handling the mouse movement only when the middle button is pressed for camera rotation.
    // If the middle button is not pressed, we let the event propagate.
    if (mMouse.IsButtonPressed(Qt::MiddleButton))
    {
        const auto Movement = pEvent->position() - mMouse.GetLastPressPosition(Qt::MiddleButton);
        mMouse.AddCumulativeMovement(Qt::MiddleButton, Movement);
        mMouse.SetLastPressPosition(Qt::MiddleButton, pEvent->position());
        return true; // Consume the event to prevent further propagation
    }

    return false; // Let the event propagate
}

bool Canavar::Engine::FreeCamera::OnLeaveEvent(QEvent*)
{
    Reset();
    return true; // Consume the event to prevent further propagation
}

bool Canavar::Engine::FreeCamera::IsKeyPressed(Qt::Key Key) const
{
    return mPressedKeys.value(Key, false);
}

float Canavar::Engine::FreeCamera::CalculateLinearSpeed(float Ifps) const
{
    if (IsKeyPressed(Qt::Key_Shift)) // If Shift is pressed, increase speed for faster movement
    {
        return mLinearSpeed * 10.0f * Ifps;
    }
    else if (IsKeyPressed(Qt::Key_Space)) // If Space is pressed, increase speed for even faster movement
    {
        return mLinearSpeed * 5.0f * Ifps;
    }
    else if (IsKeyPressed(Qt::Key_Control)) // If Control is pressed, reduce speed for precision movement
    {
        return mLinearSpeed * 0.5f * Ifps;
    }
    else // Default speed
    {
        return mLinearSpeed * Ifps;
    }
}

float Canavar::Engine::FreeCamera::CalculateAngularSpeed(float Ifps) const
{
    return mAngularSpeed * Ifps;
}

const QMap<Qt::Key, QVector3D> Canavar::Engine::FreeCamera::KEY_BINDINGS = //
    {
        { Qt::Key_W, QVector3D(0, 0, -1) }, // Forward
        { Qt::Key_S, QVector3D(0, 0, 1) },  // Backward
        { Qt::Key_A, QVector3D(-1, 0, 0) }, // Left
        { Qt::Key_D, QVector3D(1, 0, 0) },  // Right
        { Qt::Key_E, QVector3D(0, 1, 0) },  // Up
        { Qt::Key_Q, QVector3D(0, -1, 0) }  // Down
    };
