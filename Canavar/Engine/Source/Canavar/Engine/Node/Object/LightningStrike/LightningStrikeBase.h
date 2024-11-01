#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

#include <QOpenGLFunctions_4_5_Core>

namespace Canavar::Engine
{
    class Camera;
    class Shader;

    class LightningStrikeBase : public Object, protected QOpenGLFunctions_4_5_Core
    {
      protected:
        LightningStrikeBase();
        ~LightningStrikeBase();

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object) override;

      public:
        void Initialize();
        void Render(Camera* pCamera, Shader* pLightningStrikeShader, Shader* pLightningStrikeQuadGeneratorShader, float ifps);
        virtual QVector<QVector3D> GetWorldPositionsOfTerminationPoints() = 0;

      private:
        // End point of a line segment
        // A line segment contains exactly 2 end points.
        struct EndPoint
        {
            QVector3D position; // World position of this EndPoint
            float forkLevel{ 0 };
        };

        void UpdateStrikes(Shader* pLightningStrikeShader, const QVector3D& start, const QVector3D& end, float ifps);
        void RenderLineSegments(Camera* pCamera, Shader* pLightningStrikeQuadGeneratorShader, float ifps);

        // OpenGL Stuff
        uint8_t mCurrentVertexBufferIndex{ 0 };
        uint8_t mCurrentTransformFeedbackBufferIndex{ 0 };
        GLuint mVertexBuffers[2];
        GLuint mTransformFeedbacks[2];

        float mElapsedTime{ 0.0f };

        // Create EndPoint vector as a member variable in order to prevent memory fragmentation.
        QVector<EndPoint> mEndPoints;

        // Features
        DEFINE_MEMBER(float, BaseValue, 1.0f);
        DEFINE_MEMBER(float, Decay, 1.0f);
        DEFINE_MEMBER(float, JitterDisplacementMultiplier, 1.0f);
        DEFINE_MEMBER(float, ForkLengthMultiplier, 2.0f);
        DEFINE_MEMBER(float, QuadWidth, 0.05f);
        DEFINE_MEMBER(int, SubdivisionLevel, 7);
        DEFINE_MEMBER(bool, Freeze, false);

        bool mInitialized{ false };

        // Constants
        static constexpr uint8_t MAX_SUBDIVISION_LEVEL{ 9 };
        static constexpr size_t MAX_NUMBER_OF_END_POINTS{ 10000 };
    };

    using LightningStrikeBasePtr = std::shared_ptr<LightningStrikeBase>;
}