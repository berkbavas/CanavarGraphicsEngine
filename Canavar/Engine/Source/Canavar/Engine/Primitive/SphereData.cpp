#include "SphereData.h"

#include <QVector3D>

Canavar::Engine::SphereData::SphereData(float Radius, int StackCount, int SectorCount)
{
    struct Vertex
    {
        QVector3D Position;
        QVector3D Normal;
    };

    struct TriangleFace
    {
        unsigned int Idx0;
        unsigned int Idx1;
        unsigned int Idx2;
    };

    std::vector<Vertex> Vertices;
    std::vector<TriangleFace> Faces;

    // Generate vertices
    for (int i = 0; i <= StackCount; ++i)
    {
        float StackAngle = M_PI / 2 - i * (M_PI / StackCount); // from pi/2 to -pi/2
        float xy = Radius * std::cosf(StackAngle);             // r * cos(u)
        float z = Radius * std::sinf(StackAngle);              // r * sin(u)

        for (int j = 0; j <= SectorCount; ++j)
        {
            float SectorAngle = j * (2 * M_PI / SectorCount); // from 0 to 2pi

            Vertex vertex;
            vertex.Position = QVector3D(xy * std::cosf(SectorAngle), xy * std::sinf(SectorAngle), z);
            vertex.Normal = vertex.Position.normalized();
            Vertices.push_back(vertex);
        }
    }

    // Generate faces (triangles)
    for (int i = 0; i < StackCount; ++i)
    {
        for (int j = 0; j < SectorCount; ++j)
        {
            int First = (i * (SectorCount + 1)) + j;
            int Second = First + SectorCount + 1;

            if (i != 0)
            {
                Faces.push_back(TriangleFace{ static_cast<unsigned int>(First), static_cast<unsigned int>(Second), static_cast<unsigned int>(First + 1) });
            }
            if (i != (StackCount - 1))
            {
                Faces.push_back(TriangleFace{ static_cast<unsigned int>(First + 1), static_cast<unsigned int>(Second), static_cast<unsigned int>(Second + 1) });
            }
        }
    }

    mNumberOfFaces = Faces.size();

    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Faces.size() * sizeof(TriangleFace), Faces.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Canavar::Engine::SphereData::~SphereData()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}

void Canavar::Engine::SphereData::Render()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, 3 * mNumberOfFaces, GL_UNSIGNED_INT, 0); // 3 vertex per triangle
    glBindVertexArray(0);
}
