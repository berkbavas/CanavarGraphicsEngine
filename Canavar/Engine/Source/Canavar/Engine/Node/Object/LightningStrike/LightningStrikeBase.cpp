#include "LightningStrikeBase.h"

#include "Canavar/Engine/Manager/RenderingManager/Shader.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"

Canavar::Engine::LightningStrikeBase::LightningStrikeBase()
    : Object()
    , mEndPoints(MAX_NUMBER_OF_END_POINTS)
{}

Canavar::Engine::LightningStrikeBase::~LightningStrikeBase()
{
    // TODO
    // glDeleteBuffers(2, mParticleBuffers);
    // glDeleteTransformFeedbacks(2, mTransformFeedbackObjects);
}

void Canavar::Engine::LightningStrikeBase::Initialize()
{
    initializeOpenGLFunctions();

    glGenTransformFeedbacks(2, mTransformFeedbacks);
    glGenBuffers(2, mVertexBuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[i]);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffers[i]);
        glBufferData(GL_ARRAY_BUFFER, mEndPoints.size() * sizeof(EndPoint), mEndPoints.constData(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVertexBuffers[i]);
    }

    mInitialized = true;
}

void Canavar::Engine::LightningStrikeBase::Render(Camera* pCamera, Shader* pLightningStrikeShader, Shader* pLightningStrikeQuadGeneratorShader, float ifps)
{
    if (!mInitialized)
    {
        Initialize();
    }

    mElapsedTime += ifps;

    const auto& terminationPoints = GetWorldPositionsOfTerminationPoints();

    for (const auto& point : terminationPoints)
    {
        glEnable(GL_RASTERIZER_DISCARD);
        UpdateStrikes(pLightningStrikeShader, GetWorldPosition(), point, ifps);
        glDisable(GL_RASTERIZER_DISCARD);
        RenderLineSegments(pCamera, pLightningStrikeQuadGeneratorShader, ifps);
    }
}

void Canavar::Engine::LightningStrikeBase::UpdateStrikes(Shader* pLightningStrikeShader, const QVector3D& start, const QVector3D& end, float ifps)
{
    mEndPoints[0].position = start;
    mEndPoints[0].forkLevel = 0.0f;

    mEndPoints[1].position = end;
    mEndPoints[1].forkLevel = 0.0f;

    mCurrentVertexBufferIndex = 0;
    mCurrentTransformFeedbackBufferIndex = 1;

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[i]);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffers[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(EndPoint), mEndPoints.constData()); // Update only first two elements
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVertexBuffers[i]);
    }

    for (int currentSubdivisionLevel = 0; currentSubdivisionLevel < mSubdivisionLevel; ++currentSubdivisionLevel)
    {
        // Ping-pong
        mCurrentVertexBufferIndex = mCurrentTransformFeedbackBufferIndex;
        mCurrentTransformFeedbackBufferIndex = (mCurrentTransformFeedbackBufferIndex + 1) % 2;

        float ControlValue = mBaseValue * exp(-mDecay * sqrt(currentSubdivisionLevel));
        float JitterDisplacement = ControlValue * mJitterDisplacementMultiplier;
        float ForkLength = ControlValue * mForkLengthMultiplier;

        pLightningStrikeShader->Bind();

        if (mFreeze == false)
        {
            pLightningStrikeShader->SetUniformValue("gElapsedTime", mElapsedTime);
        }

        pLightningStrikeShader->SetUniformValue("gJitterDisplacement", JitterDisplacement);
        pLightningStrikeShader->SetUniformValue("gForkLength", ForkLength);
        pLightningStrikeShader->SetUniformValue("gMode", currentSubdivisionLevel % 4 < 2 ? 1 : 0);

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffers[mCurrentVertexBufferIndex]);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[mCurrentTransformFeedbackBufferIndex]);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EndPoint), 0);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(EndPoint), (void*) offsetof(EndPoint, forkLevel));

        //GLuint Query;
        //glGenQueries(1, &Query);
        //glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, Query);

        glBeginTransformFeedback(GL_LINES);

        if (currentSubdivisionLevel == 0)
        {
            glDrawArrays(GL_LINES, 0, 2);
        }
        else
        {
            glDrawTransformFeedback(GL_LINES, mTransformFeedbacks[mCurrentVertexBufferIndex]);
        }

        glEndTransformFeedback();

        //glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        //GLuint Primitives;
        //glGetQueryObjectuiv(Query, GL_QUERY_RESULT, &Primitives);
        //glDeleteQueries(1, &Query);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        pLightningStrikeShader->Release();
    }
}

void Canavar::Engine::LightningStrikeBase::RenderLineSegments(Camera* pCamera, Shader* pLightningStrikeQuadGeneratorShader, float ifps)
{
    pLightningStrikeQuadGeneratorShader->Bind();
    pLightningStrikeQuadGeneratorShader->SetUniformValue("VP", pCamera->GetViewProjectionMatrix());
    pLightningStrikeQuadGeneratorShader->SetUniformValue("viewDirection", pCamera->GetViewDirection());
    pLightningStrikeQuadGeneratorShader->SetUniformValue("quadWidth", mQuadWidth);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffers[mCurrentTransformFeedbackBufferIndex]);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EndPoint), 0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(EndPoint), (void*) offsetof(EndPoint, forkLevel));

    glDrawTransformFeedback(GL_LINES, mTransformFeedbacks[mCurrentTransformFeedbackBufferIndex]);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    pLightningStrikeQuadGeneratorShader->Release();
}