#include "Scene.h"

Canavar::Engine::Scene::Scene::Scene(const QString &SceneName, const QVector<MeshPtr> &Meshes, const QVector<TextureMaterialPtr> &Materials, SceneNodePtr RootNode)
    : mSceneName(SceneName)
    , mSceneNameStdString(mSceneName.toStdString())
    , mMeshes(Meshes)
    , mMaterials(Materials)
    , mRootNode(std::move(RootNode))
{}

const QString &Canavar::Engine::Scene::Scene::GetSceneName() const
{
    return mSceneName;
}

const std::string &Canavar::Engine::Scene::Scene::GetSceneNameStdString() const
{
    return mSceneNameStdString;
}

Canavar::Engine::SceneNodePtr Canavar::Engine::Scene::Scene::GetRootNode() const
{
    return mRootNode;
}

void Canavar::Engine::Scene::Scene::Render(TexturedModel* pTexturedModel, Shader *pShader, RenderPass RenderPass)
{
    mRootNode->Render(pTexturedModel, pShader, RenderPass, QMatrix4x4());
}
