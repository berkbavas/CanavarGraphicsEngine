#pragma once

#include "Common.h"

#include <QObject>
#include <QVector4D>

namespace Canavar {
    namespace Engine {
        class LineStrip : public QObject
        {
        public:
            explicit LineStrip(QObject* parent = nullptr);

            void AppendPoint(const QVector3D& point);
            void RemovePoint(int index);
            void RemoveAllPoints();

            const QVector<QVector3D>& GetPoints() const;
            QVector<QVector3D>& GetPoints_NonConst();

        private:
            QVector<QVector3D> mPoints;

            DEFINE_MEMBER(QVector4D, Color);
        };
    } // namespace Engine
} // namespace Canavar
