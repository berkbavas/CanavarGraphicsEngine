#include "CircleGeometry.h"

#include "Canavar/Engine/Util/Logger.h"

#include <QVector3D>

Canavar::Engine::CircleGeometry::CircleGeometry(int NumSegments)
    : mNumSegments(std::max(3, NumSegments))
{
    LOG_DEBUG("CircleGeometry::CircleGeometry: Initializing CircleGeometry...");

    const auto Vertices = CreateGeometry(mNumSegments);

    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(QVector3D), Vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    LOG_DEBUG("CircleGeometry::CircleGeometry: CircleGeometry has been initialized.");
}

Canavar::Engine::CircleGeometry::~CircleGeometry()
{
    LOG_DEBUG("CircleGeometry::~CircleGeometry: Deleting CircleGeometry...");

    if (mVAO)
    {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
    }

    if (mVBO)
    {
        glDeleteBuffers(1, &mVBO);
        mVBO = 0;
    }

    LOG_DEBUG("CircleGeometry::~CircleGeometry: CircleGeometry has been deleted.");
}

void Canavar::Engine::CircleGeometry::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_LINE_LOOP, 0, mNumSegments);
    glBindVertexArray(0);
}

QVector<QVector3D> Canavar::Engine::CircleGeometry::CreateGeometry(int NumSegments)
{
    QVector<QVector3D> Vertices;
    Vertices.reserve(NumSegments);

    for (int i = 0; i < NumSegments; i++)
    {
        float a = (i / float(NumSegments)) * 2.0f * float(M_PI);
        Vertices.append(QVector3D(std::cos(a), std::sin(a), 0.0f));
    }

    return Vertices;
}
