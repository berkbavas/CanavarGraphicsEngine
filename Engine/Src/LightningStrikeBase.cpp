#include "LightningStrikeBase.h"
#include "CameraManager.h"

Canavar::Engine::LightningStrikeBase::LightningStrikeBase()
    : Node()
    , mBaseValue(1.0f)
    , mDecay(1.0f)
    , mJitterDisplacementMultiplier(1.0f)
    , mForkLengthMultiplier(2.0f)
    , mQuadWidth(0.05f)
    , mSubdivisionLevel(7)
    , mFreeze(false)
    , mEndPoints(MAX_NUMBER_OF_END_POINTS)
{
    Create();
}

Canavar::Engine::LightningStrikeBase::~LightningStrikeBase()
{
    CleanUp();
}

void Canavar::Engine::LightningStrikeBase::Create()
{
    mShaderManager = ShaderManager::Instance();

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
}

void Canavar::Engine::LightningStrikeBase::CleanUp()
{
    // TODO
    // glDeleteBuffers(2, mParticleBuffers);
    // glDeleteTransformFeedbacks(2, mTransformFeedbackObjects);
}

void Canavar::Engine::LightningStrikeBase::Render(float ifps)
{
    mElapsedTime += ifps;

    const auto TerminationPoints = GetWorldPositionsOfTerminationPoints();

    for (const auto& TerminationPoint : TerminationPoints)
    {
        glEnable(GL_RASTERIZER_DISCARD);
        UpdateStrikes(WorldPosition(), TerminationPoint, ifps);
        glDisable(GL_RASTERIZER_DISCARD);
        RenderLineSegments(ifps);

        //qDebug() << WorldPosition() << TerminationPoint << mElapsedTime;
    }
}

void Canavar::Engine::LightningStrikeBase::UpdateStrikes(const QVector3D& start, const QVector3D& end, float ifps)
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
        //glBufferData(GL_ARRAY_BUFFER, mEndPoints.size() * sizeof(EndPoint), mEndPoints.constData(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVertexBuffers[i]);
    }


    for (int CurrentSubdivisionLevel = 0; CurrentSubdivisionLevel < mSubdivisionLevel; ++CurrentSubdivisionLevel)
    {
        // Ping-pong
        mCurrentVertexBufferIndex = mCurrentTransformFeedbackBufferIndex;
        mCurrentTransformFeedbackBufferIndex = (mCurrentTransformFeedbackBufferIndex + 1) % 2;

        float ControlValue = mBaseValue * exp(-mDecay * sqrt(CurrentSubdivisionLevel));
        float JitterDisplacement = ControlValue * mJitterDisplacementMultiplier;
        float ForkLength = ControlValue * mForkLengthMultiplier;

        mShaderManager->Bind(ShaderType::LightningStrikeShader);
        if (mFreeze == false)
        {
            mShaderManager->SetUniformValue("gElapsedTime", mElapsedTime);
        }

        mShaderManager->SetUniformValue("gTime", 1);
        mShaderManager->SetUniformValue("gJitterDisplacement", JitterDisplacement);
        mShaderManager->SetUniformValue("gForkLength", ForkLength);
        mShaderManager->SetUniformValue("gMode", CurrentSubdivisionLevel % 4 < 2 ? 1 : 0);

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffers[mCurrentVertexBufferIndex]);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[mCurrentTransformFeedbackBufferIndex]);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EndPoint), 0);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(EndPoint), (void*)offsetof(EndPoint, forkLevel));

        //GLuint Query;
        //glGenQueries(1, &Query);
        //glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, Query);

        glBeginTransformFeedback(GL_LINES);

        if (CurrentSubdivisionLevel == 0)
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

         //qDebug() << "!!!!!!!!!: " << "Current Subdivision Level:" << CurrentSubdivisionLevel << "# of Primitives:" << Primitives;

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        mShaderManager->Release();
    }

}

void Canavar::Engine::LightningStrikeBase::RenderLineSegments(float ifps)
{

    mShaderManager->Bind(ShaderType::LightningStrikeQuadGeneratorShader);
    mShaderManager->SetUniformValue("VP", CameraManager::Instance()->GetActiveCamera()->GetViewProjectionMatrix());
    mShaderManager->SetUniformValue("viewDirection", CameraManager::Instance()->GetActiveCamera()->GetViewDirection());
    mShaderManager->SetUniformValue("quadWidth", mQuadWidth);
    mShaderManager->SetUniformValue("color", QVector4D(1, 1, 1, 1));

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffers[mCurrentTransformFeedbackBufferIndex]);

  
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EndPoint), 0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(EndPoint), (void*)offsetof(EndPoint, forkLevel));

    //glDrawArrays(GL_LINES, 0, 729);
    glDrawTransformFeedback(GL_LINES, mTransformFeedbacks[mCurrentTransformFeedbackBufferIndex]);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    mShaderManager->Release();

}

void Canavar::Engine::LightningStrikeBase::SetSubdivisionLevel(uint8_t subdivisionLevel)
{
    // TODO
}

void Canavar::Engine::LightningStrikeBase::ToJson(QJsonObject& object)
{
    // TODO
}

void Canavar::Engine::LightningStrikeBase::FromJson(const QJsonObject& object)
{
    // TODO
}