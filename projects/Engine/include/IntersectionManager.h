#pragma once

#include "Manager.h"
#include "Model.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObjectFormat>

namespace Canavar {
    namespace Engine {
        class NodeManager;
        class ShaderManager;
        class ModelDataManager;

        class IntersectionManager : public Manager, protected QOpenGLExtraFunctions
        {
        private:
            IntersectionManager();

        public:
            static IntersectionManager* Instance();

            struct IntersectionResult {
                bool success;
                QVector3D point;
            };

            bool Init() override;
            IntersectionResult Raycast(const QVector3D& rayOrigin, const QVector3D& rayDirection, const QList<Model*>& includeList, const QList<Model*>& excludeList);

        private:
            NodeManager* mNodeManager;
            ShaderManager* mShaderManager;

            QOpenGLFramebufferObjectFormat mFBOFormat;
            QOpenGLFramebufferObject* mFBO;

            QMatrix4x4 mProjection;

            static const int FBO_WIDTH;
            static const int FBO_HEIGHT;
        };
    } // namespace Engine
} // namespace Canavar
