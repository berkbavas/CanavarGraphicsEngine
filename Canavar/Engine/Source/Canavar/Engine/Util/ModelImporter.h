#pragma once

#include "Canavar/Engine/Node/Object/Scene/Material.h"
#include "Canavar/Engine/Node/Object/Scene/Scene.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <QOpenGLTexture>

namespace Canavar::Engine
{
    class ModelImporter
    {
      public:
        ModelImporter() = delete;

        static std::map<std::string, ScenePtr> Import(const QString& Directory, const QStringList& Formats);

        static ScenePtr Import(const QString& SceneName, const QString& Fullpath);

        static QOpenGLTexture* CreateTexture(const QString& Path);

      private:
        static SceneNodePtr ProcessNode(ScenePtr pScene, aiNode* aiNode);
        static MeshPtr ProcessMesh(aiMesh* aiMesh);
        static MaterialPtr ProcessMaterial(const aiScene* pScene, aiMaterial* aiMaterial, const QString& Directory);
        static bool ProcessTexture(const aiScene* pScene, MaterialPtr material, aiMaterial* aiMaterial, aiTextureType aiType, TextureType Type, const QString& Directory, int Components);
        static void CalculateAABB(ScenePtr pScene);

        static QMatrix4x4 ToQMatrix4x4(const aiMatrix4x4& aiMatrix);
    };

}
