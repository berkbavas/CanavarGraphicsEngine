#pragma once

#include "CameraManager.h"
#include "LightManager.h"
#include "ShaderManager.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
    namespace Engine {
        class Sky : public Node, protected QOpenGLExtraFunctions
        {
        private:
            Sky();

        protected:
            friend class NodeManager;
            virtual void ToJson(QJsonObject& object) override;
            virtual void FromJson(const QJsonObject& object) override;

        public:
            static Sky* Instance();

            void Render();

        private:
            static QVector3D Pow(const QVector3D& a, const QVector3D& b);
            static QVector3D Exp(const QVector3D& a);
            static QVector3D Div(float t, const QVector3D& a);
            static double EvaluateSpline(const double* spline, size_t stride, double value);
            static double Evaluate(const double* dataset, size_t stride, float turbidity, float albedo, float sunTheta);
            static QVector3D HosekWilkie(float cos_theta,
                float gamma,
                float cos_gamma,
                const QVector3D& A,
                const QVector3D& B,
                const QVector3D& C,
                const QVector3D& D,
                const QVector3D& E,
                const QVector3D& F,
                const QVector3D& G,
                const QVector3D& H,
                const QVector3D& I);

        private:
            ShaderManager* mShaderManager;
            CameraManager* mCameraManager;
            LightManager* mLightManager;

            // OpenGL Stuff
            unsigned int mVAO;
            unsigned int mVBO;

            // Model
            QVector3D A, B, C, D, E, F, G, H, I;
            QVector3D Z;

            DEFINE_MEMBER(bool, Enabled);
            DEFINE_MEMBER(float, Albedo);
            DEFINE_MEMBER(float, Turbidity);
            DEFINE_MEMBER(float, NormalizedSunY);

            double* mRGB[3];
            double* mRGBRad[3];
        };
    } // namespace Engine
} // namespace Canavar
