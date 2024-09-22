#include "FirecrackerEffect.h"

#include "Helper.h"

Canavar::Engine::FirecrackerEffect::FirecrackerEffect()
    : Node()
    , mNumberOfParticles(10000)
    , mSpanAngle(140.0f)
    , mGravityDirection(0, -1, 0)
    , mGravity(9.8f)
    , mMaxLife(12.0f)
    , mMinLife(1.0f)
    , mInitialSpeed(250.0f)
    , mLoop(false)
    , mScale(1.0f)
    , mDamping(1.0f)
{
    mShaderManager = ShaderManager::Instance();
    mCameraManager = CameraManager::Instance();

    mType = Node::NodeType::FirecrackerEffect;
    mName = "Firecracker Effect";
}

Canavar::Engine::FirecrackerEffect::~FirecrackerEffect()
{
    // TODO
}

void Canavar::Engine::FirecrackerEffect::Create()
{
    initializeOpenGLFunctions();

    for (int i = 0; i < mNumberOfParticles; i++)
        mParticles << GenerateParticle();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO); 

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE), Canavar::Engine::CUBE, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.constData(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, life));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, deadAfter));
    glEnableVertexAttribArray(3);

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
}

void Canavar::Engine::FirecrackerEffect::ToJson(QJsonObject& object)
{
    Node::ToJson(object);

    QJsonObject gravityDir;
    gravityDir.insert("x", mGravityDirection.x());
    gravityDir.insert("y", mGravityDirection.y());
    gravityDir.insert("z", mGravityDirection.z());
    object.insert("gravity_direction", gravityDir);

    object.insert("number_of_particles", mNumberOfParticles);
    object.insert("span_angle", mSpanAngle);
    object.insert("gravity", mGravity);
    object.insert("max_life", mMaxLife);
    object.insert("min_life", mMinLife);
    object.insert("initial_speed", mInitialSpeed);
    object.insert("loop", mLoop);
    object.insert("scale", mScale);
    object.insert("damping", mDamping);
}

void Canavar::Engine::FirecrackerEffect::FromJson(const QJsonObject& object)
{
    Node::FromJson(object);

    // Gravity Direction
    {
        float x = object["gravity_direction"]["x"].toDouble();
        float y = object["gravity_direction"]["y"].toDouble();
        float z = object["gravity_direction"]["z"].toDouble();

        mGravityDirection = QVector3D(x, y, z);
    }

    mNumberOfParticles = object["number_of_particles"].toInt();
    mSpanAngle = object["span_angle"].toDouble();
    mGravity = object["gravity"].toDouble();
    mMaxLife = object["max_life"].toDouble();
    mMinLife = object["min_life"].toDouble();
    mInitialSpeed = object["initial_speed"].toDouble();
    mLoop = object["loop"].toBool();
    mScale = object["scale"].toDouble();
    mDamping = object["damping"].toDouble();
}

void Canavar::Engine::FirecrackerEffect::Render(float ifps)
{
    for (int i = 0; i < mParticles.size(); i++)
    {
        if (!mLoop && mParticles[i].dead)
            continue;

        auto dampingFactor = mDamping * mParticles[i].velocity.length() * mParticles[i].velocity.normalized() * ifps;

        mParticles[i].velocity += mGravity * mGravityDirection * ifps - dampingFactor;
        mParticles[i].position += mParticles[i].velocity * ifps;
        mParticles[i].life += ifps;

        if (mParticles[i].life >= mParticles[i].deadAfter)
        {
            if (mLoop)
                mParticles[i] = GenerateParticle();
            else
                mParticles[i].dead = true;
        }
    }

    mShaderManager->Bind(ShaderType::FirecrackerEffectShader);
    mShaderManager->SetUniformValue("MVP", mCameraManager->GetActiveCamera()->GetViewProjectionMatrix() * WorldTransformation());
    mShaderManager->SetUniformValue("scale", mScale);
    mShaderManager->SetUniformValue("maxLife", mMaxLife);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mParticles.size() * sizeof(Particle), mParticles.constData());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mNumberOfParticles);
    glBindVertexArray(0);

    mShaderManager->Release();
}

Canavar::Engine::FirecrackerEffect::Particle Canavar::Engine::FirecrackerEffect::GenerateParticle()
{
    Particle p;

    float theta = Helper::GenerateFloat(M_PI * mSpanAngle / 180.0f);
    float phi = Helper::GenerateFloat(2 * M_PI);

    float x = sin(theta) * cos(phi);
    float z = sin(theta) * sin(phi);
    float y = cos(theta);

    p.position = QVector3D(0, 0, 0);
    p.velocity = Helper::GenerateBetween(0.5 * mInitialSpeed, mInitialSpeed) * QVector3D(x, y, z);
    p.life = 0.0f;
    p.deadAfter = mMinLife + Helper::GenerateFloat(mMaxLife - mMinLife);
    p.dead = false;
    return p;
}