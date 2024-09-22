#include "SelectedMeshParameters.h"

Canavar::Engine::SelectedMeshParameters::SelectedMeshParameters()
    : mMesh(nullptr)
    , mScale(0.075f)
    , mRenderVertices(false)
    , mSelectedVertexID(-1)
    , mMeshStripColor(0, 1, 0, 1)
    , mVertexColor(1, 1, 1, 1)
    , mSelectedVertexColor(0, 1, 0, 1)
{}