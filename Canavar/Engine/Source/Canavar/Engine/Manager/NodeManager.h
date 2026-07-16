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

        template<typename T, typename... Args>
        T *CreateNode(Args &&...args)
        {
            static_assert(std::is_base_of_v<Camera, T> ||            //
                              std::is_base_of_v<Light, T> ||         //
                              std::is_base_of_v<TexturedModel, T> || //
                              std::is_base_of_v<GlobalNode, T>,      //
                          "T must be derived from Camera, Light, TexturedModel, or GlobalNode");

            auto pNode = std::make_unique<T>(std::forward<Args>(args)...);
            pNode->SetNodeId(mNextNodeId++);
            T *pRawPtr = pNode.get();

            mNodes.push_back(pRawPtr);

            if constexpr (std::is_base_of_v<Camera, T>)
            {
                mCameras.push_back(std::move(pNode));
                mObjects.push_back(pRawPtr);
            }
            else if constexpr (std::is_base_of_v<Light, T>)
            {
                mLights.push_back(std::move(pNode));
                mObjects.push_back(pRawPtr);

                if constexpr (std::is_same_v<T, PointLight>)
                {
                    mPointLights.push_back(pRawPtr);
                }
                else if constexpr (std::is_same_v<T, DirectionalLight>)
                {
                    mDirectionalLights.push_back(pRawPtr);
                }

                mLightManager->AddLight(pRawPtr);
            }
            else if constexpr (std::is_base_of_v<TexturedModel, T>)
            {
                mTexturedModels.push_back(std::move(pNode));
                mObjects.push_back(pRawPtr);
            }
            else if constexpr (std::is_base_of_v<GlobalNode, T>)
            {
                mGlobalNodes.push_back(std::move(pNode));
            }

            return pRawPtr;
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