#include "NozzleEffect.h"

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Node/NodeVisitor.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Util/Util.h"

void Canavar::Engine::NozzleEffect::Initialize()
{
    GenerateParticles();

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, Velocity));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, Life));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Canavar::Engine::NozzleEffect::Render(float ifps)
{
    if (mIsInitialized == false)
    {
        Initialize();
        mIsInitialized = true;
    }

    Update(ifps);

    glBindVertexArray(mVAO);
    glDrawArrays(GL_POINTS, 0, mNumberOfParticles);
    glBindVertexArray(0);
}

void Canavar::Engine::NozzleEffect::Update(float ifps)
{
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    Particle *pParticles = static_cast<Particle *>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

    for (int i = 0; i < mNumberOfParticles; ++i)
    {
        pParticles[i].Life += ifps;

        if (pParticles[i].Life > mMaxLife)
        {
            pParticles[i] = GenerateParticle();
        }
        else
        {
            float x = pParticles[i].Position.x();
            float y = pParticles[i].Position.y();
            float z = pParticles[i].Position.z();
            float radius = std::sqrt(x * x + y * y);

            float bound = -z * mMaxRadius / mMaxLength + mMaxRadius;

            if (radius > std::pow(bound, mPower))
            {
                pParticles[i] = GenerateParticle();
            }
        }

        pParticles[i].Position += ifps * pParticles[i].Velocity;
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Canavar::Engine::NozzleEffect::GenerateParticles()
{
    for (int i = 0; i < mNumberOfParticles; ++i)
    {
        mParticles << GenerateParticle();
    }
}

Canavar::Engine::NozzleEffect::Particle Canavar::Engine::NozzleEffect::GenerateParticle() const
{
    float theta = Util::GenerateRandom(2.0f * M_PI);
    float r = Util::GenerateRandom(mMaxRadius);

    Particle Particle;
    Particle.Position = QVector3D(r * std::cos(theta), r * std::sin(theta), 0);
    Particle.Life = Util::GenerateRandom(mMaxLife);
    Particle.Velocity = Util::GenerateRandomVector(0, 0, mMaxSpeed);

    return Particle;
}

void Canavar::Engine::NozzleEffect::ToJson(QJsonObject &Object)
{
    Object::ToJson(Object);

    Object.insert("max_radius", mMaxRadius);
    Object.insert("max_life", mMaxLife);
    Object.insert("max_length", mMaxLength);
    Object.insert("max_speed", mMaxSpeed);
}

void Canavar::Engine::NozzleEffect::FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes)
{
    Object::FromJson(Object, Nodes);

    mMaxRadius = Object["max_radius"].toDouble(1.0f);
    mMaxLife = Object["max_life"].toDouble(2.0f);
    mMaxLength = Object["max_length"].toDouble(10.0f);
    mMaxSpeed = Object["max_speed"].toDouble(5.0f);
}