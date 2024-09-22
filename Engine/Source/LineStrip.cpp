#include "LineStrip.h"

#include <QDebug>

Canavar::Engine::LineStrip::LineStrip(QObject* parent)
    : QObject(parent)
    , mColor(1, 0, 0, 1)
{}

void Canavar::Engine::LineStrip::AppendPoint(const QVector3D& point)
{
    if (mPoints.size() < 64)
        mPoints << point;
    else
        qWarning() << "Line strip cannot contain more than 64 points.";
}

void Canavar::Engine::LineStrip::RemovePoint(int index)
{
    mPoints.removeAt(index);
}

void Canavar::Engine::LineStrip::RemoveAllPoints()
{
    mPoints.clear();
}

const QVector<QVector3D>& Canavar::Engine::LineStrip::GetPoints() const
{
    return mPoints;
}

QVector<QVector3D>& Canavar::Engine::LineStrip::GetPoints_NonConst()
{
    return mPoints;
}