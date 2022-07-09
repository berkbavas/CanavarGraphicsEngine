#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QObject>
#include <QQuaternion>
#include <QVector3D>

class Node : public QObject
{
    Q_OBJECT
protected:
    explicit Node(QObject *parent = nullptr);
    virtual ~Node();

public:
    const QVector3D &position() const;
    void setPosition(const QVector3D &newPosition);

    const QQuaternion &rotation() const;
    void setRotation(const QQuaternion &newRotation);

    const QVector3D &scale() const;
    void setScale(const QVector3D &newScale);

    const QMatrix4x4 &transformation() const;

private:
    void updateTransformation();

protected:
    QMatrix4x4 mTransformation;
    QVector3D mPosition;
    QQuaternion mRotation;
    QVector3D mScale;
};

#endif // NODE_H
