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

        static std::map<QString, ScenePtr> Import(const QString& directory, const QStringList& formats);

        static ScenePtr Import(const QString& sceneName, const QString& fullpath);

        static QOpenGLTexture* CreateTexture(const QString& path);

      private:
        static SceneNodePtr ProcessNode(ScenePtr pScene, aiNode* aiNode);
        static MeshPtr ProcessMesh(aiMesh* aiMesh);
        static MaterialPtr ProcessMaterial(const aiScene* pScene, aiMaterial* aiMaterial, const QString& directory);
        static bool ProcessTexture(const aiScene* pScene, MaterialPtr material, aiMaterial* aiMaterial, aiTextureType aiType, TextureType type, const QString& directory);

        static QMatrix4x4 ToQMatrix4x4(const aiMatrix4x4& matrix);
    };

}
