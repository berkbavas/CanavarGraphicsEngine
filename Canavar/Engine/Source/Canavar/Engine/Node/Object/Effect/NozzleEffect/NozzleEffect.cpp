#include "NozzleEffect.h"

#include "Canavar/Engine/Util/Util.h"

Canavar::Engine::NozzleEffect::NozzleEffect()
{
    SetNodeName("Nozzle Effect");
}

void Canavar::Engine::NozzleEffect::Initialize()
{
    if (mInitialized)
    {
        return;
    }

    initializeOpenGLFunctions();

    for (int i = 0; i < mNumberOfParticles; i++)
    {
        mParticles << GenerateParticle();
    }

    constexpr QVector3D CUBE_3D[36] = { QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(-0.5f, -0.5f, 0.5f),  QVector3D(-0.5f, 0.5f, 0.5f),   //
                                        QVector3D(0.5f, 0.5f, -0.5f),   QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(-0.5f, 0.5f, -0.5f),  //
                                        QVector3D(0.5f, -0.5f, 0.5f),   QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(0.5f, -0.5f, -0.5f),  //
                                        QVector3D(0.5f, 0.5f, -0.5f),   QVector3D(0.5f, -0.5f, -0.5f),  QVector3D(-0.5f, -0.5f, -0.5f), //
                                        QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(-0.5f, 0.5f, 0.5f),   QVector3D(-0.5f, 0.5f, -0.5f),  //
                                        QVector3D(0.5f, -0.5f, 0.5f),   QVector3D(-0.5f, -0.5f, 0.5f),  QVector3D(-0.5f, -0.5f, -0.5f), //
                                        QVector3D(-0.5f, 0.5f, 0.5f),   QVector3D(-0.5f, -0.5f, 0.5f),  QVector3D(0.5f, -0.5f, 0.5f),   //
                                        QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(0.5f, -0.5f, -0.5f),  QVector3D(0.5f, 0.5f, -0.5f),   //
                                        QVector3D(0.5f, -0.5f, -0.5f),  QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(0.5f, -0.5f, 0.5f),   //
                                        QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(0.5f, 0.5f, -0.5f),   QVector3D(-0.5f, 0.5f, -0.5f),  //
                                        QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(-0.5f, 0.5f, -0.5f),  QVector3D(-0.5f, 0.5f, 0.5f),   //
                                        QVector3D(0.5f, 0.5f, 0.5f),    QVector3D(-0.5f, 0.5f, 0.5f),   QVector3D(0.5f, -0.5f, 0.5f) }; //
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_3D), CUBE_3D, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void *) 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.constData(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) 0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, direction));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, life));
    glEnableVertexAttribArray(3);

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    mInitialized = true;
}

void Canavar::Engine::NozzleEffect::Render(float ifps)
{
    if (mInitialized == false)
    {
        Initialize();
    }

    for (int i = 0; i < mParticles.size(); i++)
    {
        mParticles[i].life += ifps;

        if (mParticles[i].life >= mParticles[i].deadAfter)
        {
            mParticles[i] = GenerateParticle();
        }
    }

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mParticles.size() * sizeof(Particle), mParticles.constData());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mNumberOfParticles);
    glBindVertexArray(0);
}

void Canavar::Engine::NozzleEffect::ToJson(QJsonObject &object)
{
    Object::ToJson(object);

    object.insert("max_radius", mMaxRadius);
    object.insert("max_life", mMaxLife);
    object.insert("max_distance", mMaxDistance);
    object.insert("min_distance", mMinDistance);
    object.insert("speed", mSpeed);
    object.insert("scale", mScale);
}

void Canavar::Engine::NozzleEffect::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    Object::FromJson(object, nodes);

    mMaxRadius = object["max_radius"].toDouble(0.8f);
    mMaxLife = object["max_life"].toDouble(0.0f);
    mMaxDistance = object["max_distance"].toDouble(10.0f);
    mMinDistance = object["min_distance"].toDouble(4.0f);
    mSpeed = object["speed"].toDouble(7.0f);
    mScale = object["scale"].toDouble(0.04f);
}

Canavar::Engine::NozzleEffect::Particle Canavar::Engine::NozzleEffect::GenerateParticle()
{
    Particle p;

    float r = Util::GenerateRandom(mMaxRadius);
    float theta = Util::GenerateRandom(2.0f * M_PI);
    float distance = mMinDistance + Util::GenerateRandom(mMaxDistance - mMinDistance);
    auto end = QVector3D(0, 0, distance);

    p.initialPosition = QVector3D(r * cos(theta), r * sin(theta), 0);
    p.direction = end - p.initialPosition;
    p.life = 0.0f;
    p.deadAfter = Util::GenerateRandom(mMaxLife);
    return p;
}