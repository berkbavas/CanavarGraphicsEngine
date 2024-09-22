#pragma once

#include "Common.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QMatrix4x4>
#include <QVector3D>

namespace Canavar {
    namespace Engine {
        class AABB
        {
        public:
            AABB();

            QVector3D GetCenter() const { return (mMin + mMax) / 2.0f; }

            QMatrix4x4 GetTransformation() const;
            AABB Transform(const QMatrix4x4& transformation) const;

            void ToJson(QJsonObject& object);
            void FromJson(const QJsonObject& object);

        private:
            DEFINE_MEMBER(QVector3D, Min);
            DEFINE_MEMBER(QVector3D, Max);
        };
    } // namespace Engine
} // namespace Canavar
