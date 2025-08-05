#pragma once

#include "Canavar/Engine/Node/Global/Global.h"
#include "Canavar/Engine/Primitive/Quad.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QOpenGLExtraFunctions>
#include <QVector3D>

namespace Canavar::Engine
{
    class Shader;
    class DirectionalLight;
    class Camera;

    class Sky : public Global, protected QOpenGLExtraFunctions
    {
      public:
        Sky();
        const char* GetNodeTypeName() const override { return "Sky"; }

        void Initialize();

        void Render(Shader* pShader, DirectionalLight* pSun, Camera* pCamera);

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes) override;

      private:
        static QVector3D Pow(const QVector3D& a, const QVector3D& b);
        static QVector3D Exp(const QVector3D& a);
        static QVector3D Div(float t, const QVector3D& a);
        static double EvaluateSpline(const double* spline, size_t stride, double value);
        static double Evaluate(const double* dataset, size_t stride, float turbidity, float albedo, float sunTheta);
        static QVector3D HosekWilkie(float cos_theta, float gamma, float cos_gamma, const QVector3D& A, const QVector3D& B, const QVector3D& C, const QVector3D& D, const QVector3D& E, const QVector3D& F, const QVector3D& G, const QVector3D& H, const QVector3D& I);

        Quad* mQuad;

        // Sky Mathematical Model
        QVector3D A, B, C, D, E, F, G, H, I;
        QVector3D Z;

        double* mRGB[3];
        double* mRGBRad[3];

        DEFINE_MEMBER(float, Albedo, 0.1f);
        DEFINE_MEMBER(float, Turbidity, 4.0f);
        DEFINE_MEMBER(float, NormalizedSunY, 1.15f);
        DEFINE_MEMBER(bool, Enabled);
    };

    using SkyPtr = std::shared_ptr<Sky>;
}
