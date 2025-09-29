#include "CircleData.h"

#include <vector>

#include <QVector3D>

Canavar::Engine::CircleData::CircleData(int NumSegments)
    : mNumSegments(NumSegments)
{
    struct Vertex
    {
        QVector3D Position;
        QVector3D Normal;
    };

    std::vector<Vertex> Circle;

    for (int i = 0; i < mNumSegments; i++)
    {
        float a = (i / float(mNumSegments)) * 2.0f * float(M_PI);
        Circle.emplace_back(Vertex{ QVector3D(std::cos(a), std::sin(a), 0.0f), QVector3D(0.0f, 0.0f, 1.0f) });
    }

    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, Circle.size() * sizeof(Vertex), Circle.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Canavar::Engine::CircleData::~CircleData()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void Canavar::Engine::CircleData::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_LINE_LOOP, 0, mNumSegments);
    glBindVertexArray(0);
}
