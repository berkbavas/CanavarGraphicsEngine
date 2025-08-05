#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

#include <QOpenGLFunctions_4_3_Core>

namespace Canavar::Engine
{
    class Shader;

    class NozzleEffect : public Object, protected QOpenGLFunctions_4_3_Core
    {
      public:
        NozzleEffect();

        const char* GetNodeTypeName() const override { return "NozzleEffect"; }

        void Render(float ifps);

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes) override;

      private:
        void Initialize();
        void Update(float ifps);

#pragma pack(push, 1)
        struct Particle
        {
            QVector3D position;
            QVector3D velocity;
            float life;
        };
#pragma pack(pop)

        void GenerateParticles();
        Particle GenerateParticle() const;

        // OpenGL stuff
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };

        QVector<Particle> mParticles;

        DEFINE_MEMBER(float, MaxRadius, 1.0f);
        DEFINE_MEMBER(float, MaxLife, 2.0f);
        DEFINE_MEMBER(float, MaxLength, 10.0f);
        DEFINE_MEMBER(float, MaxSpeed, 5.0f);
        DEFINE_MEMBER(float, Power, 1.0f);
        DEFINE_MEMBER(int, NumberOfParticles, MAX_NUMBER_OF_PARTICLES);

        bool mIsInitialized{ false };
        float mTime{ 0 };

        static constexpr int MAX_NUMBER_OF_PARTICLES{ 100'000 };
    };

    using NozzleEffectPtr = std::shared_ptr<NozzleEffect>;
}
