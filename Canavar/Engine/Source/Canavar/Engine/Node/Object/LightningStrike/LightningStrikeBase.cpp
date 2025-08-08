#include "LightningStrikeBase.h"

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"

Canavar::Engine::LightningStrikeBase::LightningStrikeBase()
    : Object()
    , mEndPoints(MAX_NUMBER_OF_POINTS)
{}

Canavar::Engine::LightningStrikeBase::~LightningStrikeBase()
{
    glDeleteBuffers(2, mVertexBuffers);
    glDeleteTransformFeedbacks(2, mTransformFeedbacks);
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
        glBufferData(GL_ARRAY_BUFFER, mEndPoints.size() * sizeof(Point), mEndPoints.constData(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVertexBuffers[i]);
    }
}

void Canavar::Engine::LightningStrikeBase::Render(Camera* pCamera, Shader* pLightningStrikeShader, Shader* pLineShader, float ifps)
{
    if (!mInitialized)
    {
        Initialize();
        mInitialized = true;
    }

    mElapsedTime += ifps;

    const auto& terminationPoints = GetWorldPositionsOfTerminationPoints();

    for (const auto& point : terminationPoints)
    {
        Update(pLightningStrikeShader, GetWorldPosition(), point, ifps);
        Render(pCamera, pLineShader, ifps);
    }
}

void Canavar::Engine::LightningStrikeBase::Update(Shader* pLightningStrikeShader, const QVector3D& start, const QVector3D& end, float ifps)
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
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(Point), mEndPoints.constData()); // Update only first two elements
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVertexBuffers[i]);
    }

    for (int index = 0; index < mSubdivisionLevel; ++index)
    {
        // Ping-pong
        mCurrentVertexBufferIndex = mCurrentTransformFeedbackBufferIndex;
        mCurrentTransformFeedbackBufferIndex = (mCurrentTransformFeedbackBufferIndex + 1) % 2;

        float ControlValue = mBaseValue * std::exp(-mDecay * index);
        float JitterDisplacement = ControlValue * mJitterDisplacementMultiplier;
        float ForkLength = ControlValue * mForkLengthMultiplier;

        pLightningStrikeShader->Bind();

        if (mFreeze == false)
        {
            pLightningStrikeShader->SetUniformValue("uElapsedTime", mElapsedTime);
        }

        pLightningStrikeShader->SetUniformValue("uJitterDisplacement", JitterDisplacement);
        pLightningStrikeShader->SetUniformValue("uForkLength", ForkLength);
        pLightningStrikeShader->SetUniformValue("uMode", index % 4 < 2 ? 1 : 0);

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffers[mCurrentVertexBufferIndex]);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[mCurrentTransformFeedbackBufferIndex]);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Point), (void*) offsetof(Point, forkLevel));

        //GLuint Query;
        //glGenQueries(1, &Query);
        //glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, Query);

        glEnable(GL_RASTERIZER_DISCARD);

        glBeginTransformFeedback(GL_LINES);

        if (index == 0)
        {
            glDrawArrays(GL_LINES, 0, 2);
        }
        else
        {
            glDrawTransformFeedback(GL_LINES, mTransformFeedbacks[mCurrentVertexBufferIndex]);
        }

        glEndTransformFeedback();

        glDisable(GL_RASTERIZER_DISCARD);

        //glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        //GLuint Primitives;
        //glGetQueryObjectuiv(Query, GL_QUERY_RESULT, &Primitives);
        //glDeleteQueries(1, &Query);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        pLightningStrikeShader->Release();
    }
}

void Canavar::Engine::LightningStrikeBase::Render(Camera* pCamera, Shader* pLineShader, float ifps)
{
    pLineShader->Bind();
    pLineShader->SetUniformValue("uMVP", pCamera->GetViewProjectionMatrix());
    pLineShader->SetUniformValue("uColor", mColor);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffers[mCurrentTransformFeedbackBufferIndex]);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Point), (void*) offsetof(Point, forkLevel));

    glDrawTransformFeedback(GL_LINES, mTransformFeedbacks[mCurrentTransformFeedbackBufferIndex]);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    pLineShader->Release();
}

void Canavar::Engine::LightningStrikeBase::ToJson(QJsonObject& object)
{
    Object::ToJson(object);

    object.insert("base_value", mBaseValue);
    object.insert("decay", mDecay);
    object.insert("jitter_displacement_multiplier", mJitterDisplacementMultiplier);
    object.insert("fork_length_multiplier", mForkLengthMultiplier);
    object.insert("subdivision_level", mSubdivisionLevel);
    object.insert("freee", mFreeze);
}

void Canavar::Engine::LightningStrikeBase::FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes)
{
    Object::FromJson(object, nodes);

    mBaseValue = object["base_value"].toDouble(1.0f);
    mDecay = object["decay"].toDouble(1.0f);
    mJitterDisplacementMultiplier = object["jitter_displacement_multiplier"].toDouble(1.0f);
    mForkLengthMultiplier = object["fork_length_multiplier"].toDouble(2.0f);
    mSubdivisionLevel = object["subdivision_level"].toInt(7);
    mFreeze = object["freeze"].toBool(false);
}
