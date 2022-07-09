#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QTimer>

class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);

    QVector3D position() const;
    void setPosition(const QVector3D &newPosition);

    QQuaternion rotation() const;
    void setRotation(const QQuaternion &newRotation);

    const QMatrix4x4 &transformation();

    const QMatrix4x4 &projection() const;
    void setProjection(const QMatrix4x4 &newProjection);

    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);

private slots:
    void update();

private:
    QMatrix4x4 mProjection;
    QMatrix4x4 mTransformation;
    QVector3D mPosition;
    QQuaternion mRotation;

    QTimer mTimer;
    QMap<Qt::Key, bool> mPressedKeys;

    float mMovementSpeed;
    float mAngularSpeed;

    bool mMousePressed;
    float mMousePreviousX;
    float mMousePreviousY;
    float mMouseDeltaX;
    float mMouseDeltaY;

    static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
};

#endif // CAMERA_H
