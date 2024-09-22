#pragma once

#include "CameraManager.h"
#include "Node.h"
#include "ShaderManager.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
    namespace Engine {
        class FirecrackerEffect : public Node, protected QOpenGLExtraFunctions
        {
        private:
            friend class NodeManager;
            FirecrackerEffect();
            ~FirecrackerEffect();

            virtual void ToJson(QJsonObject& object) override;
            virtual void FromJson(const QJsonObject& object) override;

            void Create();

            struct Particle {
                QVector3D position;
                QVector3D velocity;
                float life;
                float deadAfter;
                bool dead;
            };

            Particle GenerateParticle();

        public:
            void Render(float ifps);

        private:
            ShaderManager* mShaderManager;
            CameraManager* mCameraManager;

            QVector<Particle> mParticles;
            int mNumberOfParticles;

            // OpenGL stuff
            unsigned int mVAO;
            unsigned int mVBO;
            unsigned int mPBO;

            DEFINE_MEMBER(float, SpanAngle);
            DEFINE_MEMBER(QVector3D, GravityDirection);
            DEFINE_MEMBER(float, Gravity);
            DEFINE_MEMBER(float, MaxLife);
            DEFINE_MEMBER(float, MinLife);
            DEFINE_MEMBER(float, InitialSpeed);
            DEFINE_MEMBER(bool, Loop);
            DEFINE_MEMBER(float, Scale);
            DEFINE_MEMBER(float, Damping);
        };
    } // namespace Engine
} // namespace Canavar
