#pragma once

#include "Canavar/Engine/Camera/FreeCamera.h"
#include "Canavar/Engine/Camera/PersecutorCamera.h"
#include "Canavar/Engine/GlobalNode/GlobalNode.h"
#include "Canavar/Engine/Light/DirectionalLight.h"
#include "Canavar/Engine/Light/PointLight.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Model/PrimitiveModel/PrimitiveModel.h"
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
        static constexpr bool IsAllowedType()
        {
           return std::is_base_of_v<Camera, T> ||
                  std::is_base_of_v<Light, T> ||
                  std::is_base_of_v<TexturedModel, T> ||
                  std::is_base_of_v<PrimitiveModel, T> ||
                  std::is_base_of_v<GlobalNode, T>;
        }

        /**
         * Creates a new node of type T and adds it to the appropriate list based on its type.
         * The node is assigned a unique NodeId and is managed by the NodeManager.
         * The caller receives a raw pointer to the newly created node, but the NodeManager retains ownership.
         * @tparam T The type of node to create. Must be derived from Camera, Light, TexturedModel, PrimitiveModel, or GlobalNode.
         * @param args Arguments to forward to the constructor of T.
         */
        template<typename T, typename... Args>
        T *CreateNode(Args &&...args)
        {
            static_assert(IsAllowedType<T>(), "NodeManager::CreateNode: T must be derived from Camera, Light, TexturedModel, PrimitiveModel, or GlobalNode");

            auto pNode = std::make_unique<T>(std::forward<Args>(args)...);
            pNode->SetNodeId(mNextNodeId++);

            // Store the raw pointer before moving the unique_ptr into the list
            T *pRawPtr = pNode.get();

            mNodes.push_back(pRawPtr);

            if constexpr (std::is_base_of_v<Camera, T>)
            {
                AddCamera(std::move(pNode));
            }
            else if constexpr (std::is_base_of_v<Light, T>)
            {
                AddLight(std::move(pNode));
            }
            else if constexpr (std::is_base_of_v<TexturedModel, T>)
            {
                AddTexturedModel(std::move(pNode));
            }
            else if constexpr (std::is_base_of_v<PrimitiveModel, T>)
            {
                AddPrimitiveModel(std::move(pNode));
            }
            else if constexpr (std::is_base_of_v<GlobalNode, T>)
            {
                AddGlobalNode(std::move(pNode));
            }
            else
            {
                static_assert(false, "NodeManager::CreateNode: Unsupported node type");
            }

            // Return the raw pointer to the newly created node
            // Lifetime is managed by the NodeManager, so the caller should not delete it.
            return pRawPtr;
        }

        /**
         * Removes a node from the NodeManager and its associated list based on its type.
         * The node is detached from its parent and children, and its memory is released.
         */
        void RemoveNode(Node *pNode);

        const std::list<CameraPtr> &GetCameras() const;
        const std::list<LightPtr> &GetLights() const;
        const std::list<TexturedModelPtr> &GetTexturedModels() const;

        const std::list<Node *> &GetNodes() const;
        const std::list<Object *> &GetObjects() const;
        const std::list<PointLight *> &GetPointLights() const;
        const std::list<DirectionalLight *> &GetDirectionalLights() const;
        const std::list<PrimitiveModelPtr> &GetPrimitiveModels() const;

      private:
        void AddCamera(CameraPtr &&pCamera);
        void AddLight(LightPtr &&pLight);
        void AddTexturedModel(TexturedModelPtr &&pTexturedModel);
        void AddPrimitiveModel(PrimitiveModelPtr &&pPrimitiveModel);
        void AddGlobalNode(GlobalNodePtr &&pGlobalNode);

        Renderer *mRenderer{ nullptr };
        LightManager *mLightManager{ nullptr };

        // NodeManager owns the nodes, so we use unique_ptr for automatic memory management.
        std::list<CameraPtr> mCameras;
        std::list<LightPtr> mLights;
        std::list<TexturedModelPtr> mTexturedModels;
        std::list<PrimitiveModelPtr> mPrimitiveModels;
        std::list<GlobalNodePtr> mGlobalNodes;

        // Raw pointers for easy access to nodes without ownership semantics.
        std::list<Node *> mNodes;
        std::list<Object *> mObjects;
        std::list<PointLight *> mPointLights;
        std::list<DirectionalLight *> mDirectionalLights;

        int mNextNodeId{ 1 }; // Start from 1, as 0 is reserved for UNDEFINED_NODE_ID
    };

    using NodeManagerPtr = std::unique_ptr<NodeManager>;
}