#pragma once

#include "ModelData.h"

#include <PointLight.h>
#include <QByteArray>
#include <QQuaternion>
#include <QRandomGenerator>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Canavar {
    namespace Engine {
        class Helper
        {
        private:
            Helper();

        public:
            static QByteArray GetBytes(QString path);
            static float CalculateHorizontalFovForGivenVerticalFov(float verticalFov, float width, float height);
            static float CalculateVerticalFovForGivenHorizontalFov(float horizontalFov, float width, float height);
            static QQuaternion RotateX(float angleRadians);
            static QQuaternion RotateY(float angleRadians);
            static QQuaternion RotateZ(float angleRadians);
            static float AngleBetween(const QVector3D& v1, const QVector3D& v2, const QVector3D& left);
            static float AngleBetween(const QVector3D& v1, const QVector3D& v2);
            static void GetEulerDegrees(const QQuaternion& rotation, float& yaw, float& pitch, float& roll);
            static QQuaternion ConstructFromEulerDegrees(float yaw, float pitch, float roll);
            static QQuaternion Invert(const QQuaternion& rotation);
            static ModelData* LoadModel(const QString& name, const QString& path);
            static float GenerateFloat(float bound);
            static float GenerateBetween(float lower, float upper);
            static QVector3D GenerateVec3(float x, float y, float z);
            static QOpenGLTexture* CreateTexture(const QString& path);

            static QVector<PointLight*> GetClosePointLights(const QList<PointLight*>& nodes, const QVector3D& position, int maxCount);
            static QJsonDocument LoadJson(const QString& path);
            static bool WriteTextToFile(const QString& path, const QByteArray& content);
            static bool WriteDataToFile(const QString& path, const QByteArray& content);
            static QByteArray ReadDataFromFile(const QString& path);

        private:
            static Mesh* ProcessMesh(aiMesh* aiMesh);
            static ModelDataNode* ProcessNode(ModelData* data, aiNode* aiParentNode);
            static Material* ProcessMaterial(aiMaterial* aiMaterial, const QString& directory);
            static bool ProcessTexture(Material* material, aiMaterial* aiMaterial, aiTextureType aiType, Material::TextureType type, const QString& directory);
            static QMatrix4x4 ToQMatrix(const aiMatrix4x4& matrix);
            static void CalculateAABB(ModelData* data);

            static QRandomGenerator mGenerator;
        };
    } // namespace Engine
} // namespace Canavar

