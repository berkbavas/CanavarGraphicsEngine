#include "GeometryBase.h"

Canavar::Engine::GeometryBase::~GeometryBase()
{
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
}

void Canavar::Engine::GeometryBase::Render()
{
    glBindVertexArray(mVAO);

    if (mUseEBO)
    {
        glDrawElements(mMode, mCount, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(mMode, 0, mCount);
    }

    glBindVertexArray(0);
}
