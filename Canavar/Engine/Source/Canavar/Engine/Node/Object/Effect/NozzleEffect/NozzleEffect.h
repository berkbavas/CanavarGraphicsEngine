#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class NozzleEffect : public Object, protected QOpenGLExtraFunctions
    {
        REGISTER_OBJECT_TYPE(NozzleEffect);

      public:
        NozzleEffect();

        void Render(float ifps);

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes) override;

      private:
        struct Particle
        {
            QVector3D initialPosition;
            QVector3D direction;
            float life;
            float deadAfter;
        };

        void Initialize();

        Particle GenerateParticle();

        QVector<Particle> mParticles;
        int mNumberOfParticles{ 5000 };

        // OpenGL stuff
        unsigned int mVAO{ 0 };
        unsigned int mVBO{ 0 };
        unsigned int mPBO{ 0 };

        DEFINE_MEMBER(float, MaxRadius, 0.8f);
        DEFINE_MEMBER(float, MaxLife, 0.0f);
        DEFINE_MEMBER(float, MaxDistance, 10.0f);
        DEFINE_MEMBER(float, MinDistance, 4.0f);
        DEFINE_MEMBER(float, Speed, 7.0f);
        DEFINE_MEMBER(float, Scale, 0.04f);

        DEFINE_MEMBER(bool, Initialized, false);
    };

    using NozzleEffectPtr = std::shared_ptr<NozzleEffect>;
}
