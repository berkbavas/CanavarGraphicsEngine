#include "Model.h"

Canavar::Engine::Model::Model(const QString& sceneName)
    : mSceneName(sceneName)
{
    SetNodeName(sceneName);
}

QMatrix4x4 Canavar::Engine::Model::GetMeshTransformation(const QString& meshName)
{
    return mMeshTransformations.value(meshName, QMatrix4x4());
}

void Canavar::Engine::Model::SetMeshTransformation(const QString& meshName, const QMatrix4x4& transformation)
{
    mMeshTransformations.insert(meshName, transformation);
}