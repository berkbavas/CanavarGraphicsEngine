#pragma once

#include "CameraManager.h"
#include "ShaderManager.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
    namespace Engine {
        class NozzleEffect : public Node, protected QOpenGLExtraFunctions
        {
        protected:
            friend class NodeManager;
            NozzleEffect();
            ~NozzleEffect();

            void ToJson(QJsonObject& object) override;
            void FromJson(const QJsonObject& object) override;

            void Create();

        public:
            void Render(float ifps);

        private:
            struct Particle {
                QVector3D initialPosition;
                QVector3D direction;
                float life;
                float deadAfter;
            };

            Particle GenerateParticle();

        private:
            ShaderManager* mShaderManager;
            CameraManager* mCameraManager;

            QVector<Particle> mParticles;
            int mNumberOfParticles;

            // OpenGL stuff
            unsigned int mVAO;
            unsigned int mVBO;
            unsigned int mPBO;

            DEFINE_MEMBER(float, MaxRadius);
            DEFINE_MEMBER(float, MaxLife);
            DEFINE_MEMBER(float, MaxDistance);
            DEFINE_MEMBER(float, MinDistance);
            DEFINE_MEMBER(float, Speed);
            DEFINE_MEMBER(float, Scale);
        };
    } // namespace Engine
} // namespace Canavar
