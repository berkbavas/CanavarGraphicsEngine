#pragma once

#include "Canavar/Engine/Camera/DummyCamera.h"
#include "Canavar/Engine/Camera/FreeCamera.h"
#include "Canavar/Engine/Camera/PersecutorCamera.h"
#include "Canavar/Engine/GlobalNode/GlobalNode.h"
#include "Canavar/Engine/Light/DirectionalLight.h"
#include "Canavar/Engine/Light/PointLight.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Model/PrimitiveModel/PrimitiveModel.h"
#include "Canavar/Engine/Model/TexturedModel/TexturedModel.h"
#include "Canavar/Engine/Object/DummyObject.h"

#include <list>
#include <memory>

#include <QUuid>

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
            return std::is_base_of_v<Node, T>;
        }

        /**
         * Creates a new node of type T and adds it to the appropriate list based on its type.
         * The node is assigned a unique NodeId and is managed by the NodeManager.
         * The caller receives a raw pointer to the newly created node, but the NodeManager retains ownership.
         * @tparam T The type of node to create. Must be derived from Node.
         * @param args Arguments to forward to the constructor of T.
         */
        template<typename T, typename... Args>
        T *CreateNode(Args &&...args)
        {
            static_assert(IsAllowedType<T>(), "NodeManager::CreateNode: T must be derived from Node");

            auto pNode = std::make_unique<T>(std::forward<Args>(args)...);
            pNode->SetNodeId(mNextNodeId++);

            // Store the raw pointer before moving the unique_ptr into the list
            T *pRawPtr = pNode.get();

            if constexpr (std::is_base_of_v<Camera, T>)
            {
                AddCamera(pRawPtr);
            }
            else if constexpr (std::is_base_of_v<Light, T>)
            {
                AddLight(pRawPtr);
            }
            else if constexpr (std::is_base_of_v<TexturedModel, T>)
            {
                AddTexturedModel(pRawPtr);
            }
            else if constexpr (std::is_base_of_v<PrimitiveModel, T>)
            {
                AddPrimitiveModel(pRawPtr);
            }
            else if constexpr (std::is_base_of_v<GlobalNode, T>)
            {
                AddGlobalNode(pRawPtr);
            }
            else if constexpr (std::is_base_of_v<DummyObject, T>)
            {
                AddDummyObject(pRawPtr);
            }
            else
            {
                static_assert(false, "NodeManager::CreateNode: Unsupported node type");
            }

            mNodes.push_back(std::move(pNode));

            // Return the raw pointer to the newly created node
            // Lifetime is managed by the NodeManager, so the caller should not delete it.
            return pRawPtr;
        }

        template<typename T>
        T *FindNodeByName(const std::string &Name) const
        {
            static_assert(IsAllowedType<T>(), "NodeManager::FindNodeByName: T must be derived from Node");

            for (const auto &pNode : mNodes)
            {
                if (auto *pTypedNode = dynamic_cast<T *>(pNode.get()))
                {
                    if (pTypedNode->GetNodeName() == Name)
                    {
                        return pTypedNode;
                    }
                }
            }
            return nullptr; // Not found
        }

        template<typename T>
        T *FindNodeByType() const
        {
            static_assert(IsAllowedType<T>(), "NodeManager::FindNodeByType: T must be derived from Node");

            for (const auto &pNode : mNodes)
            {
                if (auto *pTypedNode = dynamic_cast<T *>(pNode.get()))
                {
                    return pTypedNode; // Return the first node of type T found
                }
            }
            return nullptr; // Not found
        }

        /**
         * Removes a node from the NodeManager and its associated list based on its type.
         * The node is detached from its parent and children, and its memory is released.
         */
        void RemoveNode(Node *pNode);

        /**
         * Loads nodes from a JSON scene file and adds them to the NodeManager.
         * Global nodes (Sky, Haze, Terrain) and the Sun (DirectionalLight) that were already
         * created by the Renderer will have their properties updated from the file instead of
         * being duplicated.  Unsupported node types are skipped with a warning.
         * @param FilePath Path to the JSON scene file.
         * @return true on success, false if the file could not be read or parsed.
         */
        bool ImportNodes(const std::string& FilePath);

        /**
         * Serialises all nodes currently managed by the NodeManager to a JSON scene
         * file that can be re-loaded with ImportNodes.
         * Unsupported node categories (PrimitiveModel) are silently skipped.
         * @param FilePath Destination path for the JSON file.
         * @return true on success, false if the file could not be written.
         */
        bool ExportNodes(const std::string& FilePath);

        const std::list<NodePtr> &GetNodes() const;

        const std::list<Camera *> &GetCameras() const;
        const std::list<Light *> &GetLights() const;
        const std::list<TexturedModel *> &GetTexturedModels() const;
        const std::list<Object *> &GetObjects() const;
        const std::list<PointLight *> &GetPointLights() const;
        const std::list<DirectionalLight *> &GetDirectionalLights() const;
        const std::list<PrimitiveModel *> &GetPrimitiveModels() const;
        const std::list<GlobalNode *> &GetGlobalNodes() const;
        const std::list<DummyObject *> &GetDummyObjects() const;

      private:
        void AddCamera(Camera *pCamera);
        void AddLight(Light *pLight);
        void AddTexturedModel(TexturedModel *pTexturedModel);
        void AddPrimitiveModel(PrimitiveModel *pPrimitiveModel);
        void AddGlobalNode(GlobalNode *pGlobalNode);
        void AddDummyObject(DummyObject *pDummyObject);

        Renderer *mRenderer{ nullptr };
        LightManager *mLightManager{ nullptr };

        // NodeManager owns the nodes, so we use unique_ptr for automatic memory management.
        std::list<NodePtr> mNodes;

        // Raw pointers for easy access to nodes without ownership semantics.
        std::list<Camera *> mCameras;
        std::list<Light *> mLights;
        std::list<TexturedModel *> mTexturedModels;
        std::list<PrimitiveModel *> mPrimitiveModels;
        std::list<GlobalNode *> mGlobalNodes;
        std::list<DummyObject *> mDummyObjects;
        std::list<Object *> mObjects;
        std::list<PointLight *> mPointLights;
        std::list<DirectionalLight *> mDirectionalLights;

        int mNextNodeId{ 1 }; // Start from 1, as 0 is reserved for UNDEFINED_NODE_ID
    };

    using NodeManagerPtr = std::unique_ptr<NodeManager>;
}