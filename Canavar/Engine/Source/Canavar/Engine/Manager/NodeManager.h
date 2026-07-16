#pragma once

#include "Canavar/Engine/Camera/FreeCamera.h"
#include "Canavar/Engine/Camera/PersecutorCamera.h"
#include "Canavar/Engine/GlobalNode/GlobalNode.h"
#include "Canavar/Engine/Light/DirectionalLight.h"
#include "Canavar/Engine/Light/PointLight.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Model/TexturedModel/TexturedModel.h"

#include <list>
#include <memory>

namespace Canavar::Engine
{
    class Renderer;

    class NodeManager : public Manager
    {
      public:
        explicit NodeManager(Renderer *pRenderer);
        virtual ~NodeManager() = default;

        void Initialize();

        template<typename T>
        T *CreateCamera()
        {
            static_assert(std::is_base_of<Camera, T>::value, "T must be derived from Camera");
            auto pCamera = std::make_unique<T>();
            pCamera->SetNodeId(mNextNodeId++);

            T *pCameraRawPtr = pCamera.get();

            mCameras.push_back(std::move(pCamera));
            mNodes.push_back(pCameraRawPtr);
            mObjects.push_back(pCameraRawPtr);

            return pCameraRawPtr;
        }

        template<typename T>
        T *CreateLight()
        {
            static_assert(std::is_base_of<Light, T>::value, "T must be derived from Light");
            auto pLight = std::make_unique<T>();
            pLight->SetNodeId(mNextNodeId++);
            T *pLightRawPtr = pLight.get();

            mLights.push_back(std::move(pLight));
            mNodes.push_back(pLightRawPtr);
            mObjects.push_back(pLightRawPtr);

            if constexpr (std::is_same<T, PointLight>::value)
            {
                mPointLights.push_back(pLightRawPtr);
            }
            else if constexpr (std::is_same<T, DirectionalLight>::value)
            {
                mDirectionalLights.push_back(pLightRawPtr);
            }

            mLightManager->AddLight(pLightRawPtr);

            return pLightRawPtr;
        }

        template<typename T>
        T *CreateTexturedModel(const QString &ModelName)
        {
            static_assert(std::is_base_of<TexturedModel, T>::value, "T must be derived from TexturedModel");
            auto pTexturedModel = std::make_unique<T>(ModelName);
            pTexturedModel->SetNodeId(mNextNodeId++);

            T *pTexturedModelRawPtr = pTexturedModel.get();
            mTexturedModels.push_back(std::move(pTexturedModel));
            mNodes.push_back(pTexturedModelRawPtr);
            mObjects.push_back(pTexturedModelRawPtr);

            return pTexturedModelRawPtr;
        }

        template<typename T, typename... Args>
        T *CreateGlobalNode(Args&&... args)
        { 
            static_assert(std::is_base_of<GlobalNode, T>::value, "T must be derived from GlobalNode");
            auto pGlobalNode = std::make_unique<T>(std::forward<Args>(args)...);
            pGlobalNode->SetNodeId(mNextNodeId++);

            T *pGlobalNodeRawPtr = pGlobalNode.get();
            mNodes.push_back(pGlobalNodeRawPtr);
            mGlobalNodes.push_back(std::move(pGlobalNode));

            return pGlobalNodeRawPtr;
        }

        void RemoveNode(Node *pNode);

        const std::list<CameraPtr> &GetCameras() const;
        const std::list<LightPtr> &GetLights() const;
        const std::list<TexturedModelPtr> &GetTexturedModels() const;

        const std::list<Node *> &GetNodes() const;
        const std::list<Object *> &GetObjects() const;
        const std::list<PointLight *> &GetPointLights() const;
        const std::list<DirectionalLight *> &GetDirectionalLights() const;

      private:
        Renderer *mRenderer{ nullptr };
        LightManager *mLightManager{ nullptr };

        std::list<CameraPtr> mCameras;
        std::list<LightPtr> mLights;
        std::list<TexturedModelPtr> mTexturedModels;
        std::list<GlobalNodePtr> mGlobalNodes;

        std::list<Node *> mNodes;
        std::list<Object *> mObjects;
        std::list<PointLight *> mPointLights;
        std::list<DirectionalLight *> mDirectionalLights;

        int mNextNodeId{ 1 }; // Start from 1, as 0 is reserved for UNDEFINED_NODE_ID
    };

    using NodeManagerPtr = std::unique_ptr<NodeManager>;
}