#include "Common.h"

#include <QVector2D>
#include <QVector3D>

QVector3D const Canavar::Engine::CUBE[36] = {QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(-0.5f, -0.5f, 0.5f),  QVector3D(-0.5f, 0.5f, 0.5f),   //
                                             QVector3D(0.5f, 0.5f, -0.5f),   QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(-0.5f, 0.5f, -0.5f),  //
                                             QVector3D(0.5f, -0.5f, 0.5f),   QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(0.5f, -0.5f, -0.5f),  //
                                             QVector3D(0.5f, 0.5f, -0.5f),   QVector3D(0.5f, -0.5f, -0.5f),  QVector3D(-0.5f, -0.5f, -0.5f), //
                                             QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(-0.5f, 0.5f, 0.5f),   QVector3D(-0.5f, 0.5f, -0.5f),  //
                                             QVector3D(0.5f, -0.5f, 0.5f),   QVector3D(-0.5f, -0.5f, 0.5f),  QVector3D(-0.5f, -0.5f, -0.5f), //
                                             QVector3D(-0.5f, 0.5f, 0.5f),   QVector3D(-0.5f, -0.5f, 0.5f),  QVector3D(0.5f, -0.5f, 0.5f),   //
                                             QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(0.5f, -0.5f, -0.5f),  QVector3D(0.5f, 0.5f, -0.5f),   //
                                             QVector3D(0.5f, -0.5f, -0.5f),  QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(0.5f, -0.5f, 0.5f),   //
                                             QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(0.5f, 0.5f, -0.5f),   QVector3D(-0.5f, 0.5f, -0.5f),  //
                                             QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(-0.5f, 0.5f, -0.5f),  QVector3D(-0.5f, 0.5f, 0.5f),   //
                                             QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(-0.5f, 0.5f, 0.5f),   QVector3D(0.5f, -0.5f, 0.5f)};  //

QVector2D const Canavar::Engine::QUAD[12] = {QVector2D(-1.0f, +1.0f),
                                             QVector2D(0.0f, 1.0f),
                                             QVector2D(-1.0f, -1.0f),
                                             QVector2D(0.0f, 0.0f),
                                             QVector2D(+1.0f, -1.0f),
                                             QVector2D(1.0f, 0.0f),
                                             QVector2D(-1.0f, +1.0f),
                                             QVector2D(0.0f, 1.0f),
                                             QVector2D(+1.0f, -1.0f),
                                             QVector2D(1.0f, 0.0f),
                                             QVector2D(+1.0f, +1.0f),
                                             QVector2D(1.0f, 1.0f)};

QVector3D const Canavar::Engine::CUBE_STRIP[17] = {
    QVector3D(-0.5f, -0.5f, -0.5f),
    QVector3D(-0.5f, -0.5f, 0.5f),
    QVector3D(-0.5f, 0.5f, 0.5f),
    QVector3D(-0.5f, 0.5f, -0.5f),
    QVector3D(-0.5f, -0.5f, -0.5f),
    QVector3D(0.5f, -0.5f, -0.5f),
    QVector3D(0.5f, 0.5f, -0.5f),
    QVector3D(-0.5f, 0.5f, -0.5f),
    QVector3D(-0.5f, 0.5f, 0.5f),
    QVector3D(0.5f, 0.5f, 0.5f),
    QVector3D(0.5f, 0.5f, -0.5f),
    QVector3D(0.5f, -0.5f, -0.5f),
    QVector3D(0.5f, -0.5f, 0.5f),
    QVector3D(0.5f, 0.5f, 0.5f),
    QVector3D(-0.5f, 0.5f, 0.5f),
    QVector3D(-0.5f, -0.5f, 0.5f),
    QVector3D(0.5f, -0.5f, 0.5f),
};
