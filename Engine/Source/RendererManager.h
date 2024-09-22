#pragma once

#include "Camera.h"
#include "LineStrip.h"
#include "Manager.h"
#include "OpenGLVertexArrayObject.h"
#include "SelectedMeshParameters.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObjectFormat>

namespace Canavar {
    namespace Engine {
        class NodeManager;
        class CameraManager;
        class LightManager;
        class ShaderManager;
        class ModelDataManager;
        class Haze;
        class Sky;
        class Terrain;
        class PointLight;
        class Sun;
        class Config;

        class RendererManager : public Manager, protected QOpenGLExtraFunctions
        {
        private:
            RendererManager();

        public:
            static RendererManager* Instance();

            bool Init() override;
            void Resize(int width, int height) override;

            void Render(float ifps) override;

            void AddSelectableNode(Node* node, QVector4D color = QVector4D(1, 1, 1, 1));
            void RemoveSelectableNode(Node* node);

            void AddSelectedMesh(Model* model, const SelectedMeshParameters& parameters);
            void RemoveSelectedMesh(Model* model);

            void AddLineStrip(LineStrip* lineStrip);
            void RemoveLineStrip(LineStrip* lineStrip);

            SelectedMeshParameters GetSelectedMeshParameters(Model* model) const;
            SelectedMeshParameters& GetSelectedMeshParameters_Ref(Model* model);

            const QMap<Model*, SelectedMeshParameters>& GetSelectedMeshes() const;

        private:
            enum class FramebufferType { //
                Default,
                Temporary,
                Ping,
                Pong,
            };

            void SetCommonUniforms();
            void DeleteFramebuffers();
            void CreateFramebuffers(int width, int height);

            void OnSelectedNodeDestroyed(QObject* node);
            void OnSelectedModelDestroyed(QObject* model);
            void OnLineStripDestroyed(QObject* lineStrip);

        private:
            NodeManager* mNodeManager;
            CameraManager* mCameraManager;
            LightManager* mLightManager;
            ShaderManager* mShaderManager;
            ModelDataManager* mModelDataManager;

            Camera* mCamera;
            Sun* mSun;
            Sky* mSky;
            Haze* mHaze;
            Terrain* mTerrain;
            Config* mConfig;

            QVector<PointLight*> mClosePointLights;

            QMap<Node*, QVector4D> mSelectableNodes; // Nodes whose AABB to be rendered -> Line color
            QMap<Model*, SelectedMeshParameters> mSelectedMeshes;

            QList<LineStrip*> mLineStrips;

            QMap<FramebufferType, QOpenGLFramebufferObject*> mFBOs;
            QMap<FramebufferType, QOpenGLFramebufferObjectFormat*> mFBOFormats;

            int mWidth;
            int mHeight;

            DEFINE_MEMBER(int, BlurPass);
            DEFINE_MEMBER(float, Exposure);
            DEFINE_MEMBER(float, Gamma);

            OpenGLVertexArrayObject mQuad;
            OpenGLVertexArrayObject mCube;
            OpenGLVertexArrayObject mCubeStrip;
            OpenGLVertexArrayObject mLineStripHandle;

            GLuint mColorAttachments[2];
        };
    } // namespace Engine
} // namespace Canavar
