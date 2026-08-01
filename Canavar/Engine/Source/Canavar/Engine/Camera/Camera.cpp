#include "Camera.h"

QVector3D Canavar::Engine::Camera::ComputeRayFromScreen(const QPointF &Point) const
{
    // Convert screen position to Normalized Device Coordinates (NDC)
    float NdcX = (2.0f * Point.x()) / mWidth - 1.0f;
    float NdcY = 1.0f - (2.0f * Point.y()) / mHeight;
    float NdcZ = 1.0f; // Assuming we want the far plane

    QVector4D ClipCoords(NdcX, NdcY, NdcZ, 1.0f);

    // Inverse projection to get eye/view space coordinates
    QMatrix4x4 InverseProjection = GetProjectionMatrix().inverted();
    QVector4D EyeCoords = InverseProjection * ClipCoords;
    EyeCoords.setZ(-1.0f); // forward direction in view space
    EyeCoords.setW(0.0f);  // direction vector, not a position

    // Inverse view to get world space direction
    QMatrix4x4 InvView = GetViewMatrix().inverted();
    QVector4D WorldCoords = InvView * EyeCoords;

    QVector3D WorldPosition(WorldCoords.x(), WorldCoords.y(), WorldCoords.z());
    WorldPosition.normalize();

    return WorldPosition;
}

QVector2D Canavar::Engine::Camera::ProjectWorldToScreenSpace(const QVector3D &Pos3D) const
{
    QMatrix4x4 ViewProjectionMatrix = GetViewProjectionMatrix();

    QVector4D ClipSpacePos = ViewProjectionMatrix * QVector4D(Pos3D, 1.0f);

    if (ClipSpacePos.w() == 0.0f)
        return QVector2D(-1, -1); // Avoid division by zero

    // Perform perspective divide to get Normalized Device Coordinates (NDC)
    QVector3D NDC;
    NDC.setX(ClipSpacePos.x() / ClipSpacePos.w());
    NDC.setY(ClipSpacePos.y() / ClipSpacePos.w());
    NDC.setZ(ClipSpacePos.z() / ClipSpacePos.w());

    // Convert NDC to screen coordinates
    float ScreenX = (NDC.x() + 1.0f) * 0.5f * mWidth;
    float ScreenY = (1.0f - NDC.y()) * 0.5f * mHeight; // Invert Y for screen space

    return QVector2D(ScreenX, ScreenY);
}

QVector3D Canavar::Engine::Camera::ProjectScreenToWorldSpace(const QPointF &Point, float Depth) const
{
    // Convert screen position to Normalized Device Coordinates (NDC)
    float NdcX = (2.0f * Point.x()) / mWidth - 1.0f;
    float NdcY = 1.0f - (2.0f * Point.y()) / mHeight;
    float NdcZ = Depth; // Depth in NDC space (0 to 1)

    QVector4D ClipCoords(NdcX, NdcY, NdcZ, 1.0f);

    // Inverse projection to get eye/view space coordinates
    QMatrix4x4 InverseProjection = GetProjectionMatrix().inverted();
    QVector4D EyeCoords = InverseProjection * ClipCoords;
    EyeCoords.setZ(-1.0f); // forward direction in view space
    EyeCoords.setW(0.0f);  // direction vector, not a position

    // Inverse view to get world space direction
    QMatrix4x4 InvView = GetViewMatrix().inverted();
    QVector4D WorldCoords = InvView * EyeCoords;

    QVector3D WorldPosition(WorldCoords.x(), WorldCoords.y(), WorldCoords.z());
    WorldPosition.normalize();

    return WorldPosition;
}

float Canavar::Engine::Camera::GetDepthOfPoint(const QVector3D &Point) const
{
    QMatrix4x4 ViewProjectionMatrix = GetViewProjectionMatrix();
    QVector4D ClipSpacePos = ViewProjectionMatrix * QVector4D(Point, 1.0f);

    if (ClipSpacePos.w() == 0.0f)
        return -1.0f; // Avoid division by zero

    // Perform perspective divide to get Normalized Device Coordinates (NDC)
    float NDCZ = ClipSpacePos.z() / ClipSpacePos.w();

    return NDCZ;
}

QMatrix4x4 Canavar::Engine::Camera::GetViewProjectionMatrix() const
{
    return GetProjectionMatrix() * GetViewMatrix();
}

QMatrix4x4 Canavar::Engine::Camera::GetViewMatrix() const
{
    QMatrix4x4 ViewMatrix;
    ViewMatrix.rotate(GetWorldRotation().conjugated());
    ViewMatrix.translate(-GetWorldPosition());
    return ViewMatrix;
}

QMatrix4x4 Canavar::Engine::Camera::GetRotationMatrix() const
{
    constexpr QVector4D ZERO_TRANSLATION(0, 0, 0, 1);
    QMatrix4x4 ViewMatrix = GetViewMatrix();
    ViewMatrix.setColumn(3, ZERO_TRANSLATION); // Extract rotation
    return ViewMatrix;
}

QVector3D Canavar::Engine::Camera::GetViewDirection() const
{
    return GetWorldRotation() * NEGATIVE_Z; // The view direction is the negative Z axis in the camera's local space
}

QVector3D Canavar::Engine::Camera::GetDirection(ViewDirection ViewDirection) const
{
    switch (ViewDirection)
    {
    case ViewDirection::Front:
        return GetWorldRotation() * NEGATIVE_Z;
    case ViewDirection::Back:
        return GetWorldRotation() * POSITIVE_Z;
    case ViewDirection::Up:
        return GetWorldRotation() * POSITIVE_Y;
    case ViewDirection::Down:
        return GetWorldRotation() * NEGATIVE_Y;
    case ViewDirection::Left:
        return GetWorldRotation() * NEGATIVE_X;
    case ViewDirection::Right:
        return GetWorldRotation() * POSITIVE_X;
    default:
        return QVector3D(0, 0, 0); // Default case, should not happen
    }
}

int Canavar::Engine::Camera::GetWidth() const
{
    return mWidth;
}

int Canavar::Engine::Camera::GetHeight() const
{
    return mHeight;
}

QVector2D Canavar::Engine::Camera::GetViewportSize() const
{
    return QVector2D(static_cast<float>(mWidth), static_cast<float>(mHeight));
}

void Canavar::Engine::Camera::Resize(int Width, int Height)
{
    mWidth = Width;
    mHeight = Height;
}
