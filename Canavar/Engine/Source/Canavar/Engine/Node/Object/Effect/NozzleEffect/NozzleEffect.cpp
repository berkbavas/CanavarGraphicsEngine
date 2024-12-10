#include "NozzleEffect.h"

#include "Canavar/Engine/Manager/RenderingManager/Shader.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Util/Util.h"

Canavar::Engine::NozzleEffect::NozzleEffect()
{
    SetNodeName("Nozzle Effect");
}

void Canavar::Engine::NozzleEffect::Initialize()
{
    GenerateParticles();

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, velocity));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, life));
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
        pParticles[i].life += ifps;

        if (pParticles[i].life > mMaxLife)
        {
            pParticles[i] = GenerateParticle();
        }
        else
        {
            float x = pParticles[i].position.x();
            float y = pParticles[i].position.y();
            float z = pParticles[i].position.z();
            float radius = std::sqrt(x * x + y * y);

            float bound = -z * mMaxRadius / mMaxLength + mMaxRadius;

            if (radius  > bound)
            {
                pParticles[i] = GenerateParticle();
            }
        }

        pParticles[i].position += ifps * pParticles[i].velocity;
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

    Particle particle;
    particle.position = QVector3D(r * std::cos(theta), r * std::sin(theta), 0);
    particle.life = Util::GenerateRandom(mMaxLife);
    particle.velocity = Util::GenerateRandomVector(0, 0, mMaxSpeed);

    return particle;
}

void Canavar::Engine::NozzleEffect::ToJson(QJsonObject &object)
{
    Object::ToJson(object);

    object.insert("max_radius", mMaxRadius);
    object.insert("max_life", mMaxLife);
    object.insert("max_length", mMaxLength);
    object.insert("max_speed", mMaxSpeed);
}

void Canavar::Engine::NozzleEffect::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    Object::FromJson(object, nodes);

    mMaxRadius = object["max_radius"].toDouble(1.0f);
    mMaxLife = object["max_life"].toDouble(2.0f);
    mMaxLength = object["max_length"].toDouble(10.0f);
    mMaxSpeed = object["max_speed"].toDouble(5.0f);
}