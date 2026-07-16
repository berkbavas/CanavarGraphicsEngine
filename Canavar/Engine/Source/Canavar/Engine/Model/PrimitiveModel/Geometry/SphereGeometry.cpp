#include "SphereGeometry.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::SphereGeometry::SphereGeometry(float Radius, int StackCount, int SectorCount)
{
    LOG_DEBUG("SphereGeometry::SphereGeometry: Initializing SphereGeometry...");

    CreateGeometry(Radius, StackCount, SectorCount);

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mFaces.size() * sizeof(SphereGeometry::TriangleFace), mFaces.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(SphereGeometry::Vertex), mVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SphereGeometry::Vertex), (void *) offsetof(SphereGeometry::Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SphereGeometry::Vertex), (void *) offsetof(SphereGeometry::Vertex, Normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    mVertices.clear();
    mFaces.clear();

    LOG_DEBUG("SphereGeometry::SphereGeometry: SphereGeometry has been initialized.");
}

Canavar::Engine::SphereGeometry::~SphereGeometry()
{
    LOG_DEBUG("SphereGeometry::~SphereGeometry: Deleting OpenGL stuff...");

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

    if (mEBO)
    {
        glDeleteBuffers(1, &mEBO);
        mEBO = 0;
    }

    LOG_DEBUG("SphereGeometry::~SphereGeometry: OpenGL stuff has been deleted.");
}

void Canavar::Engine::SphereGeometry::Render()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, 3 * mNumberOfFaces, GL_UNSIGNED_INT, 0); // 3 vertex per triangle
    glBindVertexArray(0);
}

void Canavar::Engine::SphereGeometry::CreateGeometry(float Radius, int StackCount, int SectorCount)
{
    Vertex TempVertex;

    for (int i = 0; i <= StackCount; ++i)
    {
        float stackAngle = M_PI / 2 - i * M_PI / StackCount; // from pi/2 to -pi/2
        float xy = Radius * cosf(stackAngle);
        float z = Radius * sinf(stackAngle);

        TempVertex.Position.setZ(z);

        for (int j = 0; j <= SectorCount; ++j)
        {
            float sectorAngle = j * 2 * M_PI / SectorCount;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            float nx = x / Radius;
            float ny = y / Radius;
            float nz = z / Radius;

            TempVertex.Position.setX(x);
            TempVertex.Position.setY(y);

            TempVertex.Normal.setX(nx);
            TempVertex.Normal.setY(ny);
            TempVertex.Normal.setZ(nz);

            mVertices << TempVertex;
        }
    }

    // Indices
    for (int i = 0; i < StackCount; ++i)
    {
        unsigned int k1 = i * (SectorCount + 1); // start of current stack
        unsigned int k2 = k1 + SectorCount + 1;  // start of next stack

        for (int j = 0; j < SectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                mFaces.push_back(TriangleFace{ k1, k2, k1 + 1 });
            }

            if (i != (StackCount - 1))
            {
                mFaces.push_back(TriangleFace{ k1 + 1, k2, k2 + 1 });
            }
        }
    }

    mNumberOfFaces = mFaces.size();
}
