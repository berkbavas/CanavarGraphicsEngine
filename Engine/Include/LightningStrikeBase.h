#pragma once

#include "NodeManager.h"
#include "ShaderManager.h"

#include <QVector3D>
#include <QOpenGLFunctions_4_4_Core>

namespace Canavar::Engine
{
    class LightningStrikeBase : public Node, protected QOpenGLFunctions_4_4_Core
    {
    protected:
        LightningStrikeBase();
        ~LightningStrikeBase() override;

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object) override;

    public:
        void SetSubdivisionLevel(uint8_t subdivisionLevel);
        void Render(float ifps);
        virtual QVector<QVector3D> GetWorldPositionsOfTerminationPoints() = 0;

        // Features
        DEFINE_MEMBER(float, BaseValue);
        DEFINE_MEMBER(float, Decay);
        DEFINE_MEMBER(float, JitterDisplacementMultiplier);
        DEFINE_MEMBER(float, ForkLengthMultiplier);
        DEFINE_MEMBER(float, QuadWidth);
        DEFINE_MEMBER(int, SubdivisionLevel);
        DEFINE_MEMBER(bool, Freeze);

    private:
        void Create();
        void CleanUp();

        void UpdateStrikes(const QVector3D& start, const QVector3D& end, float ifps);

        // For debugging
        void RenderLineSegments(float ifps);


        // End point of a line segment
        // A line segment contains exactly 2 end points.
        struct EndPoint
        {
            QVector3D position; // World position of this EndPoint
            float forkLevel{ 0 };
        };

        // OpenGL Stufff
        uint8_t mCurrentVertexBufferIndex{ 0 };
        uint8_t mCurrentTransformFeedbackBufferIndex{ 0 };
        GLuint mVertexBuffers[2];
        GLuint mTransformFeedbacks[2];

        float mElapsedTime{ 0.0f };

        // Create EndPoint vector as a member variable in order to prevent memory fragmentation.
        QVector<EndPoint> mEndPoints;

        ShaderManager* mShaderManager;

        // Constants
        static constexpr uint8_t MAX_SUBDIVISION_LEVEL{ 9 };
        static constexpr size_t MAX_NUMBER_OF_END_POINTS{ 10000 };
    };
}