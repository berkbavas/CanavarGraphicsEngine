#pragma once

#include "Canavar/Engine/Scene/Scene.h"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <QMap>
#include <QOpenGLTexture>
#include <QString>

namespace Canavar::Engine
{
    class AssimpModelImporter final
    {
      public:
        AssimpModelImporter() = delete;

        static QMap<QString, ScenePtr> Import(const QString& ScenesDirectory, const QStringList& Formats);

      private:
        static ScenePtr Import(const QString& SceneName, const QString& SceneDirPath, const QString& SceneFilePath);
        static SceneNodePtr ProcessNode(const QVector<MeshPtr>& SceneMeshes, aiNode* aiNode);
        static MeshPtr ProcessMesh(aiMesh* aiMesh, int MeshId, TextureMaterialPtr pTextureMaterial);
        static QVector<Vertex> ProcessVertices(aiMesh* aiMesh);
        static QVector<TriangleFace> ProcessTriangleFaces(aiMesh* aiMesh);
        static TextureMaterialPtr ProcessMaterial(const aiScene* pScene, aiMaterial* aiMaterial, const QString& Directory);
        static bool ProcessTexture(const aiScene* pScene, TextureMaterialPtr pTextureMaterial, aiMaterial* aiMaterial, aiTextureType aiType, TextureType TextureType, const QString& Directory, int ComponentCount);
        static float CalculateMaterialOpacity(aiMaterial* aiMaterial);
        static AABB CalculateMeshBoundingBox(aiMesh* aiMesh);
    };
}