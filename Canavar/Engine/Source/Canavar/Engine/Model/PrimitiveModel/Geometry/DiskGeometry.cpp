#include "DiskGeometry.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::DiskGeometry::DiskGeometry(float Radius, int SegmentCount)
{
    LOG_DEBUG("DiskGeometry::DiskGeometry: Initializing DiskGeometry...");

    CreateGeometry(Radius, SegmentCount);

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mFaces.size() * sizeof(DiskGeometry::TriangleFace), mFaces.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(DiskGeometry::Vertex), mVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DiskGeometry::Vertex), (void *) offsetof(DiskGeometry::Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DiskGeometry::Vertex), (void *) offsetof(DiskGeometry::Vertex, Normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    mVertices.clear();
    mFaces.clear();

    LOG_DEBUG("DiskGeometry::DiskGeometry: DiskGeometry has been initialized.");
}

Canavar::Engine::DiskGeometry::~DiskGeometry()
{
    LOG_DEBUG("DiskGeometry::~DiskGeometry: Deleting OpenGL stuff...");

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

    LOG_DEBUG("DiskGeometry::~DiskGeometry: OpenGL stuff has been deleted.");
}

void Canavar::Engine::DiskGeometry::Render()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, 3 * mNumberOfFaces, GL_UNSIGNED_INT, 0); // 3 vertex per triangle
    glBindVertexArray(0);
}

void Canavar::Engine::DiskGeometry::CreateGeometry(float Radius, int SegmentCount)
{
    mNumberOfFaces = 0;
    mVertices.clear();
    mFaces.clear();

    // Center vertex
    mVertices.push_back(Vertex{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } });

    // Create rim vertices
    for (int i = 0; i < SegmentCount; ++i)
    {
        float Angle = 2.0f * M_PI * i / SegmentCount;
        float x = Radius * std::cos(Angle);
        float y = Radius * std::sin(Angle);
        mVertices.push_back(Vertex{ { x, y, 0.0f }, { 0.0f, 0.0f, 1.0f } });
    }

    // Create faces (triangles)
    for (int i = 0; i < SegmentCount; ++i)
    {
        const unsigned int Next = (i + 1) % SegmentCount;
        const unsigned int CenterIndex = 0;         // The center vertex is at index 0
        const unsigned int RimIndex = i + 1;        // Rim vertices start from index 1
        const unsigned int NextRimIndex = Next + 1; // Next rim vertex
        mFaces.push_back(TriangleFace{ CenterIndex, RimIndex, NextRimIndex });
        ++mNumberOfFaces;
    }
}
