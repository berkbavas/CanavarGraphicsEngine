#pragma once

#include "Node.h"

namespace Canavar {
    namespace Engine {
        class ModelData;

        class Model : public Node
        {
        protected:
            friend class NodeManager;
            Model(const QString& modelName);

            virtual void ToJson(QJsonObject& object) override;
            virtual void FromJson(const QJsonObject& object) override;

        public:
            QMatrix4x4 GetMeshTransformation(const QString& meshName);
            void SetMeshTransformation(const QString& meshName, const QMatrix4x4& transformation);

            QVector4D GetMeshOverlayColor(const QString& meshName);
            void SetMeshOverlayColor(const QString& meshName, const QVector4D& color);

            float GetMeshOverlayColorFactor(const QString& meshName);
            void SetMeshOverlayColorFactor(const QString& meshName, float factor);

            const QString& GetModelName() const { return mModelName; }

            void Render(RenderMode renderMode);

        private:
            void UpdateAABB();

        protected:
            QMap<QString, QMatrix4x4> mMeshTransformations;
            QMap<QString, QVector4D> mMeshOverlayColors;
            QMap<QString, float> mMeshOverlayColorFactors;
            QString mModelName;
            ModelData* mData;

            DEFINE_MEMBER(QVector4D, Color);
            DEFINE_MEMBER(QVector4D, OverlayColor);
            DEFINE_MEMBER(float, OverlayColorFactor);
            DEFINE_MEMBER(float, Ambient);
            DEFINE_MEMBER(float, Diffuse);
            DEFINE_MEMBER(float, Specular);
            DEFINE_MEMBER(float, Shininess);
        };
    } // namespace Engine
} // namespace Canavar
