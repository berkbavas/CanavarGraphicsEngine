#include "Sky.h"

#include "Canavar/Engine/Manager/RenderingManager/Shader.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"

#include <QFile>
#include <QVector3D>
#include <QtMath>

// Sky model belongs to Hosek-Wilkie - An Analytic Model for Full Spectral Sky-Dome Radiance
// Code is taken from https://github.com/diharaw/sky-models

void Canavar::Engine::Sky::Initialize()
{
    SetNodeName("Sky");

    initializeOpenGLFunctions();

    mQuad = new Quad;

    mRGB[0] = new double[1080];
    mRGB[1] = new double[1080];
    mRGB[2] = new double[1080];

    mRGBRad[0] = new double[120];
    mRGBRad[1] = new double[120];
    mRGBRad[2] = new double[120];

    {
        QFile file(":/Resources/Sky/SkyRGB.data");
        QDataStream stream(&file);

        if (file.open(QIODevice::ReadOnly))
        {
            for (int i = 0; i < 1080; i++)
            {
                stream >> mRGB[0][i];
                stream >> mRGB[1][i];
                stream >> mRGB[2][i];
            }
        }
        else
        {
            CGE_EXIT_FAILURE("Sky::Sky:: Could not open SkyRGB.data");
        }
    }

    {
        QFile file(":/Resources/Sky/SkyRGBRad.data");
        QDataStream stream(&file);

        if (file.open(QIODevice::ReadOnly))
        {
            for (int i = 0; i < 120; i++)
            {
                stream >> mRGBRad[0][i];
                stream >> mRGBRad[1][i];
                stream >> mRGBRad[2][i];
            }
        }
        else
        {
            CGE_EXIT_FAILURE("Sky::Sky:: Could not open SkyRGBRad.data");
        }
    }
}

void Canavar::Engine::Sky::Render(Shader* pShader, DirectionalLight* pSun, Camera* pCamera)
{
    if (mEnabled == false)
    {
        return;
    }

    const auto sunDirection = pSun->GetDirection().normalized();
    const auto sunTheta = std::acos(std::clamp(0.f, -sunDirection.y(), 1.f));

    for (int i = 0; i < 3; ++i)
    {
        A[i] = Evaluate(mRGB[i] + 0, 9, mTurbidity, mAlbedo, sunTheta);
        B[i] = Evaluate(mRGB[i] + 1, 9, mTurbidity, mAlbedo, sunTheta);
        C[i] = Evaluate(mRGB[i] + 2, 9, mTurbidity, mAlbedo, sunTheta);
        D[i] = Evaluate(mRGB[i] + 3, 9, mTurbidity, mAlbedo, sunTheta);
        E[i] = Evaluate(mRGB[i] + 4, 9, mTurbidity, mAlbedo, sunTheta);
        F[i] = Evaluate(mRGB[i] + 5, 9, mTurbidity, mAlbedo, sunTheta);
        G[i] = Evaluate(mRGB[i] + 6, 9, mTurbidity, mAlbedo, sunTheta);

        // Swapped in the dataset
        H[i] = Evaluate(mRGB[i] + 8, 9, mTurbidity, mAlbedo, sunTheta);
        I[i] = Evaluate(mRGB[i] + 7, 9, mTurbidity, mAlbedo, sunTheta);

        Z[i] = Evaluate(mRGBRad[i], 1, mTurbidity, mAlbedo, sunTheta);
    }

    if (mNormalizedSunY)
    {
        QVector3D S = HosekWilkie(cos(sunTheta), 0, 1.f, A, B, C, D, E, F, G, H, I) * Z;
        Z /= QVector3D::dotProduct(S, QVector3D(0.2126, 0.7152, 0.0722));
        Z *= mNormalizedSunY;
    }

    glDisable(GL_DEPTH_TEST);

    pShader->Bind();
    pShader->SetUniformValue("nodeId", static_cast<float>(GetNodeId()));
    pShader->SetUniformValue("IVP", pCamera->GetRotationMatrix().inverted() * pCamera->GetProjectionMatrix().inverted());
    pShader->SetUniformValue("skyYOffset", pCamera->CalculateSkyYOffset(30000.0f));
    pShader->SetUniformValue("sunDirection", sunDirection);
    pShader->SetUniformValue("A", A);
    pShader->SetUniformValue("B", B);
    pShader->SetUniformValue("C", C);
    pShader->SetUniformValue("D", D);
    pShader->SetUniformValue("E", E);
    pShader->SetUniformValue("F", F);
    pShader->SetUniformValue("G", G);
    pShader->SetUniformValue("H", H);
    pShader->SetUniformValue("I", I);
    pShader->SetUniformValue("Z", Z);

    mQuad->Render();

    glEnable(GL_DEPTH_TEST);
}

void Canavar::Engine::Sky::ToJson(QJsonObject& object)
{
    GlobalNode::ToJson(object);

    object.insert("albedo", mAlbedo);
    object.insert("turbidity", mTurbidity);
    object.insert("normalized_sun_y", mNormalizedSunY);
}

void Canavar::Engine::Sky::FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes)
{
    GlobalNode::FromJson(object, nodes);

    mAlbedo = object["albedo"].toDouble(0.1f);
    mTurbidity = object["turbidity"].toDouble(4.0f);
    mNormalizedSunY = object["normalized_sun_y"].toDouble(1.15f);
}

QVector3D Canavar::Engine::Sky::Pow(const QVector3D& a, const QVector3D& b)
{
    return QVector3D(std::pow(a.x(), b.x()), std::pow(a.y(), b.y()), std::pow(a.z(), b.z()));
}

QVector3D Canavar::Engine::Sky::Exp(const QVector3D& a)
{
    return QVector3D(std::exp(a.x()), std::exp(a.y()), std::exp(a.z()));
}

QVector3D Canavar::Engine::Sky::Div(float t, const QVector3D& a)
{
    return QVector3D(t / a.x(), t / a.y(), t / a.z());
}

double Canavar::Engine::Sky::EvaluateSpline(const double* spline, size_t stride, double value)
{
    return                                                                      //
        std::pow(1 - value, 5) * spline[0 * stride] * 1 +                       //
        std::pow(1 - value, 4) * std::pow(value, 1) * spline[1 * stride] * 5 +  //
        std::pow(1 - value, 3) * std::pow(value, 2) * spline[2 * stride] * 10 + //
        std::pow(1 - value, 2) * std::pow(value, 3) * spline[3 * stride] * 10 + //
        std::pow(1 - value, 1) * std::pow(value, 4) * spline[4 * stride] * 5 +  //
        std::pow(value, 5) * spline[5 * stride] * 1;                            //
                                                                                //
}

double Canavar::Engine::Sky::Evaluate(const double* dataset, size_t stride, float turbidity, float albedo, float sunTheta)
{
    // Splines are functions of elevation^1/3
    double elevationK = std::pow(std::max<float>(0.f, 1.f - sunTheta / (M_PI / 2.f)), 1.f / 3.0f);

    // Table has values for turbidity 1..10
    int turbidity0 = qBound(1, static_cast<int>(turbidity), 10);
    int turbidity1 = qMin(turbidity0 + 1, 10);
    float turbidityK = qBound(0.f, turbidity - turbidity0, 1.f);

    const double* datasetA0 = dataset;
    const double* datasetA1 = dataset + stride * 6 * 10;

    double a0t0 = EvaluateSpline(datasetA0 + stride * 6 * (turbidity0 - 1), stride, elevationK);
    double a1t0 = EvaluateSpline(datasetA1 + stride * 6 * (turbidity0 - 1), stride, elevationK);
    double a0t1 = EvaluateSpline(datasetA0 + stride * 6 * (turbidity1 - 1), stride, elevationK);
    double a1t1 = EvaluateSpline(datasetA1 + stride * 6 * (turbidity1 - 1), stride, elevationK);

    return a0t0 * (1 - albedo) * (1 - turbidityK) + a1t0 * albedo * (1 - turbidityK) + a0t1 * (1 - albedo) * turbidityK + a1t1 * albedo * turbidityK;
}

QVector3D Canavar::Engine::Sky::HosekWilkie(float cos_theta, float gamma, float cos_gamma, const QVector3D& A, const QVector3D& B, const QVector3D& C, const QVector3D& D, const QVector3D& E, const QVector3D& F, const QVector3D& G, const QVector3D& H, const QVector3D& I)
{
    auto chi = Div((1.f + cos_gamma * cos_gamma), //
                   Pow(QVector3D(1, 1, 1) + (H * H) - 2.f * cos_gamma * H, QVector3D(1.5f, 1.5f, 1.5f)));

    return (QVector3D(1, 1, 1) + A * Exp(B / (cos_theta + 0.01f))) //
           * (C + D * Exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * (float) sqrt(std::max(0.f, cos_theta)));
}