#include "Gui.h"
#include "Config.h"
#include "Haze.h"
#include "Helper.h"
#include "IntersectionManager.h"
#include "ModelDataManager.h"
#include "RendererManager.h"
#include "SelectableNodeRenderer.h"

#include <QFileDialog>
#include <QJsonDocument>

Canavar::Engine::Gui::Gui(QObject* parent)
    : QObject(parent)
    , mSelectedNode(nullptr)
    , mSelectedModel(nullptr)
    , mSelectedMesh(nullptr)
    , mSelectedVertexIndex(-1)
    , mDrawAllBBs(false)
    , mNodeSelectionEnabled(false)
    , mMeshSelectionEnabled(false)
    , mVertexSelectionEnabled(false)
    , mWidth(1600)
    , mHeight(900)
    , mMoveEnabled(false)
    , mLockNode(false)
    , mLockAxisX(false)
    , mLockAxisY(false)
    , mLockAxisZ(false)

{
    mCreatableNodeNames << "Dummy Node"
        << "Model"
        << "Free Camera"
        << "Dummy Camera"
        << "Point Light"
        << "Nozzle Effect"
        << "Firecracker Effect"
        << "Persecutor Camera"
        << "Lightning Strike Generator"
        << "Lightning Strike Attractor"
        << "Lightning Strike Spherical";

    mNodeManager = NodeManager::Instance();

    connect(mNodeManager, &NodeManager::NodeCreated, this, [=](Canavar::Engine::Node* node) {
        if (mDrawAllBBs)
            RendererManager::Instance()->AddSelectableNode(node, QVector4D(1, 1, 1, 1));
        });

    connect(
        this,
        &Gui::ShowFileDialog,
        this,
        [=]() { //
            QString path = QFileDialog::getSaveFileName(nullptr, "Save as JSON", "", "*.json");
            if (!path.isEmpty())
            {
                QJsonDocument document;
                QJsonObject object;
                mNodeManager->ToJson(object);
                document.setObject(object);
                QByteArray content = document.toJson(QJsonDocument::Indented);
                Helper::WriteTextToFile(path, content);
            }
        },
        Qt::QueuedConnection);

    //mLineStrip = new LineStrip;
    //mLineStrip->AppendPoint(QVector3D(0, 10, 0));
    //mLineStrip->AppendPoint(QVector3D(0, 0, 0));

    //RendererManager::Instance()->AddLineStrip(mLineStrip);

    mCube = NodeManager::Instance()->CreateModel("Cube");
    mCube->SetColor(QVector4D(0, 1, 0, 1));

    mRayDirection = QVector3D(0, -1, 0);
}

void Canavar::Engine::Gui::Draw()
{
    // Intersection Debug
    {
        //ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        //ImGui::Begin("Intersection Manager Debug");

        //ImGui::TextColored(ImVec4(1, 1, 0, 1), "Origin");
        //ImGui::DragFloat("x##IntersectionManagerOrigin", &mLineStrip->GetPoints_NonConst()[0][0], 0.1f, -100, 100);
        //ImGui::DragFloat("y##IntersectionManagerOrigin", &mLineStrip->GetPoints_NonConst()[0][1], 0.1f, -100, 100);
        //ImGui::DragFloat("z##IntersectionManagerOrigin", &mLineStrip->GetPoints_NonConst()[0][2], 0.1f, -100, 100);

        //ImGui::TextColored(ImVec4(1, 1, 0, 1), "Ray Direction");
        //ImGui::SliderFloat("x##IntersectionManagerRayDirection", &mRayDirection[0], -1, 1);
        //ImGui::SliderFloat("y##IntersectionManagerRayDirection", &mRayDirection[1], -1, 1);
        //ImGui::SliderFloat("z##IntersectionManagerRayDirection", &mRayDirection[2], -1, 1);

        //auto result = IntersectionManager::Instance()->Raycast(mLineStrip->GetPoints().at(0), mRayDirection, QList<Model*>(), { mCube });

        //mLineStrip->GetPoints_NonConst()[1] = 1000 * mRayDirection;

        //if (result.success)
        //    mCube->SetWorldPosition(result.point);

        //ImGui::End();
    }

    // Render Settings
    {
        ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("Render Settings");

        ImGui::SliderFloat("Exposure##RenderSettings", &RendererManager::Instance()->GetExposure_NonConst(), 0.01f, 2.0f, "%.3f");
        ImGui::SliderFloat("Gamma##RenderSettings", &RendererManager::Instance()->GetGamma_NonConst(), 0.01f, 4.0f, "%.3f");
        ImGui::SliderInt("Bloom Blur Pass##RenderSettings", &RendererManager::Instance()->GetBlurPass_NonConst(), 0, 100);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // Create Node
    {
        ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("Create Node");

        // Select a node type
        if (ImGui::BeginCombo("Choose a node type", mSelectedNodeName.toStdString().c_str()))
        {
            for (const auto& name : mCreatableNodeNames)
                if (ImGui::Selectable(name.toStdString().c_str()))
                    mSelectedNodeName = name;

            ImGui::EndCombo();
        }

        if (mSelectedNodeName == "Model")
        {
            const auto& modelNames = ModelDataManager::Instance()->GetModelNames();

            if (ImGui::BeginCombo("Choose a 3D Model", mSelectedModelName.toStdString().c_str()))
            {
                for (const auto& name : modelNames)
                    if (ImGui::Selectable(name.toStdString().c_str()))
                        mSelectedModelName = name;

                ImGui::EndCombo();
            }
        }

        if (ImGui::Button("Create Node"))
        {
            Node* node = nullptr;

            if (mSelectedNodeName == "Dummy Node")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::DummyNode);
            else if (mSelectedNodeName == "Free Camera")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::FreeCamera);
            else if (mSelectedNodeName == "Dummy Camera")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::DummyCamera);
            else if (mSelectedNodeName == "Point Light")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::PointLight);
            else if (mSelectedNodeName == "Nozzle Effect")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::NozzleEffect);
            else if (mSelectedNodeName == "Firecracker Effect")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::FirecrackerEffect);
            else if (mSelectedNodeName == "Lightning Strike Generator")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::LightningStrikeGenerator);
            else if (mSelectedNodeName == "Lightning Strike Attractor")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::LightningStrikeAttractor);
            else if (mSelectedNodeName == "Lightning Strike Spherical")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::LightningStrikeSpherical);
            else if (mSelectedNodeName == "Persecutor Camera")
                node = NodeManager::Instance()->CreateNode(Node::NodeType::PersecutorCamera);
            else if (mSelectedNodeName == "Model" && !mSelectedModelName.isEmpty())
                node = NodeManager::Instance()->CreateModel(mSelectedModelName);

            if (node)
            {
                auto position = CameraManager::Instance()->GetActiveCamera()->WorldPosition();
                auto viewDir = CameraManager::Instance()->GetActiveCamera()->GetViewDirection();

                node->SetWorldPosition(position + 10 * viewDir);
            }
        }

        ImGui::End();
    }

    // Nodes
    {
        ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("Nodes", NULL, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Export World to JSON File"))
                    emit ShowFileDialog();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        const auto& nodes = NodeManager::Instance()->GetNodes();

        ImGui::Text("Selection");

        // Draw All Bounding Boxes
        if (ImGui::Checkbox("Draw All Bounding Boxes", &mDrawAllBBs))
        {
            if (mDrawAllBBs)
                for (const auto& node : nodes)
                    RendererManager::Instance()->AddSelectableNode(node, QVector4D(1, 1, 1, 1));
            else
                for (const auto& node : nodes)
                    RendererManager::Instance()->RemoveSelectableNode(node);

            if (mNodeSelectionEnabled)
                if (mSelectedNode)
                    RendererManager::Instance()->AddSelectableNode(mSelectedNode, QVector4D(1, 0, 0, 1));
        }

        // Node Selection
        {
            ImGui::BeginDisabled(!Config::Instance()->GetNodeSelectionEnabled());

            if (ImGui::Checkbox("Node Selection", &mNodeSelectionEnabled))
            {
                if (mNodeSelectionEnabled)
                {
                    RendererManager::Instance()->AddSelectableNode(mSelectedNode, QVector4D(1, 0, 0, 1));
                }
                else
                {
                    if (mDrawAllBBs)
                        RendererManager::Instance()->AddSelectableNode(mSelectedNode, QVector4D(1, 1, 1, 1));
                    else
                        for (const auto& node : nodes)
                            RendererManager::Instance()->RemoveSelectableNode(node);

                    mMeshSelectionEnabled = false;
                    mVertexSelectionEnabled = false;
                    mLockNode = false;
                    SetSelectedMesh(nullptr);
                }
            }

            // Lock Node
            ImGui::BeginDisabled(!mSelectedNode);
            ImGui::SameLine();
            ImGui::Checkbox("Lock Node", &mLockNode);
            ImGui::EndDisabled();

            // Mesh Selection
            ImGui::BeginDisabled(!mSelectedModel || !mNodeSelectionEnabled);
            if (ImGui::Checkbox("Mesh Selection", &mMeshSelectionEnabled))
            {
                SetSelectedMesh(nullptr);
                mVertexSelectionEnabled = false;
            }
            ImGui::EndDisabled();

            // Vertex Selection
            ImGui::BeginDisabled(!mSelectedMesh || !mMeshSelectionEnabled);
            if (ImGui::Checkbox("Vertex Selection", &mVertexSelectionEnabled))
            {
                RendererManager::Instance()->GetSelectedMeshParameters_Ref(mSelectedModel).mRenderVertices = true;
                SetSelectedVertexIndex(-1);
            }
            ImGui::EndDisabled();

            ImGui::EndDisabled();
        }

        // Move
        ImGui::Text("Move");
        ImGui::Checkbox("Move enabled", &mMoveEnabled);
        ImGui::BeginDisabled(!mMoveEnabled);
        ImGui::Checkbox("Move only on x-Axis", &mLockAxisX);
        ImGui::Checkbox("Move only on y-Axis", &mLockAxisY);
        ImGui::Checkbox("Move only on z-Axis", &mLockAxisZ);
        ImGui::EndDisabled();

        // Select a node
        ImGui::Text("Nodes");

        if (ImGui::BeginCombo("Select a node", mSelectedNode ? mSelectedNode->GetName().toStdString().c_str() : "-"))
        {
            for (int i = 0; i < nodes.size(); ++i)
                if (ImGui::Selectable((nodes[i]->GetName() + "##" + nodes[i]->GetUUID()).toStdString().c_str()))
                {
                    mMeshSelectionEnabled = false;
                    mVertexSelectionEnabled = false;
                    SetSelectedNode(nodes[i]);
                }

            ImGui::EndCombo();
        }

        // Actions
        if (mSelectedNode)
        {
            if (!ImGui::CollapsingHeader("Actions"))
            {
                ImGui::Text("Type: %d", mSelectedNode->GetType());
                ImGui::Text("ID: %d", mSelectedNode->GetID());
                ImGui::Text("UUID: %s", mSelectedNode->GetUUID().toStdString().c_str());

                // Assign a parent
                {
                    if (ImGui::BeginCombo("Assign a parent", mSelectedNode->GetParent() ? mSelectedNode->GetParent()->GetName().toStdString().c_str() : "-"))
                    {
                        if (ImGui::Selectable("-"))
                            mSelectedNode->SetParent(nullptr);

                        for (int i = 0; i < nodes.size(); ++i)
                        {
                            if (mSelectedNode == nodes[i])
                                continue;

                            if (ImGui::Selectable(nodes[i]->GetName().toStdString().c_str()))
                                mSelectedNode->SetParent(nodes[i]);
                        }

                        ImGui::EndCombo();
                    }
                }

                if (ImGui::Button("Remove this node"))
                {
                    NodeManager::Instance()->RemoveNode(mSelectedNode);
                    mMeshSelectionEnabled = false;
                    mVertexSelectionEnabled = false;
                }
            }
        }

        if (mSelectedNode)
        {
            switch (mSelectedNode->GetType())
            {
            case Canavar::Engine::Node::NodeType::DummyNode:
                Draw(mSelectedNode);
                break;
            case Canavar::Engine::Node::NodeType::PersecutorCamera:
            case Canavar::Engine::Node::NodeType::DummyCamera:
                Draw(dynamic_cast<PerspectiveCamera*>(mSelectedNode));
                Draw(mSelectedNode);
                break;
            case Canavar::Engine::Node::NodeType::FreeCamera:
                Draw(dynamic_cast<FreeCamera*>(mSelectedNode));
                Draw(dynamic_cast<PerspectiveCamera*>(mSelectedNode));
                Draw(mSelectedNode);
                break;
            case Canavar::Engine::Node::NodeType::Sky:
                Draw(dynamic_cast<Sky*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::Sun:
                Draw(dynamic_cast<Sun*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::Model:
                Draw(mSelectedNode);
                Draw(dynamic_cast<Model*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::Terrain:
                Draw(dynamic_cast<Terrain*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::Haze:
                Draw(dynamic_cast<Haze*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::PointLight:
                Draw(dynamic_cast<Light*>(mSelectedNode));
                Draw(dynamic_cast<PointLight*>(mSelectedNode));
                Draw(dynamic_cast<Node*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::NozzleEffect:
                Draw(dynamic_cast<NozzleEffect*>(mSelectedNode));
                Draw(dynamic_cast<Node*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::FirecrackerEffect:
                Draw(dynamic_cast<FirecrackerEffect*>(mSelectedNode));
                Draw(dynamic_cast<Node*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::LightningStrikeGenerator:
                Draw(dynamic_cast<LightningStrikeGenerator*>(mSelectedNode));
                Draw(dynamic_cast<Node*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::LightningStrikeAttractor:
                Draw(dynamic_cast<LightningStrikeAttractor*>(mSelectedNode));
                Draw(dynamic_cast<Node*>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::LightningStrikeSpherical:
                Draw(dynamic_cast<LightningStrikeSpherical*>(mSelectedNode));
                Draw(dynamic_cast<Node*>(mSelectedNode));
                break;
            default:
                break;
            }
        }

        ImGui::End();
    }
}

void Canavar::Engine::Gui::Draw(Node* node)
{
    // Position
    if (!ImGui::CollapsingHeader("Position##Node"))
    {
        float x = node->Position().x();
        float y = node->Position().y();
        float z = node->Position().z();

        if (ImGui::DragFloat("x##NodePosition", &x, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->SetPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodePosition", &y, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->SetPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodePosition", &z, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->SetPosition(QVector3D(x, y, z));
    }

    // Rotation
    if (ImGui::CollapsingHeader("Rotation##Node"))
    {
        float yaw, pitch, roll;

        Canavar::Engine::Helper::GetEulerDegrees(node->Rotation(), yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 360.0f, "%.3f"))
            node->SetRotation(Canavar::Engine::Helper::ConstructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            node->SetRotation(Canavar::Engine::Helper::ConstructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -180.0f, 180.0f, "%.3f"))
            node->SetRotation(Canavar::Engine::Helper::ConstructFromEulerDegrees(yaw, pitch, roll));
    }

    // World Position
    if (ImGui::CollapsingHeader("World Position##Node"))
    {
        QVector3D position = node->WorldPosition();
        float x = position.x();
        float y = position.y();
        float z = position.z();

        if (ImGui::DragFloat("x##NodeWorldPosition", &x, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->SetWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodeWorldPosition", &y, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->SetWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodeWorldPosition", &z, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->SetWorldPosition(QVector3D(x, y, z));
    }

    // World rotation
    if (ImGui::CollapsingHeader("World Rotation##Node"))
    {
        QQuaternion rotation = node->WorldRotation();
        float yaw, pitch, roll;

        Helper::GetEulerDegrees(rotation, yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 360.0f, "%.3f"))
            node->SetWorldRotation(Helper::ConstructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            node->SetWorldRotation(Helper::ConstructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -180.0f, 180.0f, "%.3f"))
            node->SetWorldRotation(Helper::ConstructFromEulerDegrees(yaw, pitch, roll));
    }

    // Scale
    if (ImGui::CollapsingHeader("Scale##Node"))
    {
        QVector3D scale = node->Scale();
        float x = scale.x();
        float y = scale.y();
        float z = scale.z();
        float all = qMin(x, qMin(y, z));

        if (ImGui::DragFloat("All##NodeScale", &all, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->SetScale(QVector3D(all, all, all));
        if (ImGui::DragFloat("x##NodeScale", &x, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->SetScale(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodeScale", &y, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->SetScale(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodeScale", &z, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->SetScale(QVector3D(x, y, z));
    }
}

void Canavar::Engine::Gui::Draw(Model* model)
{
    if (!ImGui::CollapsingHeader("Shading Parameters##Model"))
    {
        ImGui::SliderFloat("Ambient##Model", &model->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Model", &model->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Model", &model->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess##Model", &model->GetShininess_NonConst(), 1.0f, 128.0f, "%.3f");
        ImGui::ColorEdit4("Color##Model", (float*)&model->GetColor_NonConst());
        ImGui::SliderFloat("Overlay Color Factor##Model", &model->GetOverlayColorFactor_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::ColorEdit4("Overlay Color##Model", (float*)&model->GetOverlayColor_NonConst());
    }

    if (!ImGui::CollapsingHeader("Meshes##Model"))
    {
        if (auto data = ModelDataManager::Instance()->GetModelData(model->GetModelName()))
        {
            const auto& meshes = data->GetMeshes();

            if (ImGui::BeginCombo("Select a mesh", mSelectedMesh ? mSelectedMesh->GetName().toStdString().c_str() : "-"))
            {
                for (int i = 0; i < meshes.size(); ++i)
                    if (ImGui::Selectable(meshes[i]->GetName().toStdString().c_str()))
                        SetSelectedMesh(meshes[i]);

                ImGui::EndCombo();
            }

            if (mSelectedMesh)
            {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh Info");
                ImGui::Text("ID: %d", mSelectedMesh->GetID());
                ImGui::Text("Number of Vertices: %d", mSelectedMesh->GetNumberOfVertices());

                auto transformation = model->GetMeshTransformation(mSelectedMesh->GetName());
                auto position = transformation.column(3);
                auto rotation = QQuaternion::fromRotationMatrix(transformation.normalMatrix());

                // Position
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Position");
                ImGui::DragFloat("x##MeshPosition", &position[0], 0.01f, -1000.0f, 1000.0f, "%.3f");
                ImGui::DragFloat("y##MeshPosition", &position[1], 0.01f, -1000.0f, 1000.0f, "%.3f");
                ImGui::DragFloat("z##MeshPosition", &position[2], 0.01f, -1000.0f, 1000.0f, "%.3f");

                // Rotation
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Rotation");
                float yaw, pitch, roll;

                Canavar::Engine::Helper::GetEulerDegrees(rotation, yaw, pitch, roll);

                if (ImGui::SliderFloat("Yaw##MeshRotation", &yaw, 0.0f, 360.0f, "%.3f"))
                    rotation = Canavar::Engine::Helper::ConstructFromEulerDegrees(yaw, pitch, roll);
                if (ImGui::SliderFloat("Pitch##MeshRotation", &pitch, -89.999f, 89.999f, "%.3f"))
                    rotation = Canavar::Engine::Helper::ConstructFromEulerDegrees(yaw, pitch, roll);
                if (ImGui::SliderFloat("Roll##MeshRotation", &roll, -180.0f, 180.0f, "%.3f"))
                    rotation = Canavar::Engine::Helper::ConstructFromEulerDegrees(yaw, pitch, roll);

                transformation.setToIdentity();
                transformation.setColumn(3, position);
                transformation.rotate(rotation);
                model->SetMeshTransformation(mSelectedMesh->GetName(), transformation);

                if (mSelectedVertexIndex != -1)
                {
                    Mesh::Vertex vertex = mSelectedMesh->GetVertex(mSelectedVertexIndex);
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Vertex Info");
                    ImGui::Text("Selected vertex index: %d", mSelectedVertexIndex);
                    ImGui::Text("Selected vertex position: (%.4f, %.4f, %.4f)", vertex.position[0], vertex.position[1], vertex.position[2]);
                }
            }
        }
    }
}

void Canavar::Engine::Gui::Draw(Sky* node)
{
    if (!ImGui::CollapsingHeader("Sky"))
    {
        ImGui::SliderFloat("Albedo##Sky", &node->GetAlbedo_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Turbidity##Sky", &node->GetTurbidity_NonConst(), 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("Normalized Sun Y##Sun", &node->GetNormalizedSunY_NonConst(), 0.0f, 10.0f, "%.3f");
    }
}

void Canavar::Engine::Gui::Draw(Sun* sun)
{
    if (!ImGui::CollapsingHeader("Sun"))
    {
        ImGui::Text("Direction:");
        float x = sun->GetDirection().x();
        float y = sun->GetDirection().y();
        float z = sun->GetDirection().z();
        float r = sun->GetDirection().length();
        float theta = qRadiansToDegrees(atan2(z, x));
        float phi = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

        if (qFuzzyCompare(abs(phi), 90.0f))
            theta = 0.0f;

        bool updated = false;

        if (ImGui::SliderFloat("Theta##DirectionalLight", &theta, -180.0f, 180.0f, "%.1f"))
            updated = true;

        if (ImGui::SliderFloat("Phi##DirectionalLight", &phi, -90.0f, 90.0f, "%.1f"))
            updated = true;

        if (updated)
        {
            x = r * cos(qDegreesToRadians(phi)) * cos(qDegreesToRadians(theta));
            y = r * sin(qDegreesToRadians(phi));
            z = r * cos(qDegreesToRadians(phi)) * sin(qDegreesToRadians(theta));

            sun->SetDirection(QVector3D(x, y, z));
        }

        ImGui::Text("Shading Parameters:");
        ImGui::SliderFloat("Ambient##Sun", &sun->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Sun", &sun->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Sun", &sun->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::ColorEdit4("Color##Sun", (float*)&sun->GetColor_NonConst());
    }
}

void Canavar::Engine::Gui::Draw(PerspectiveCamera* camera)
{
    if (!ImGui::CollapsingHeader("Parameters##PerspectiveCamera"))
    {
        ImGui::SliderFloat("FOV##PerspectiveCamera", &camera->GetVerticalFov_NonConst(), 1.0f, 120.0);
        ImGui::SliderFloat("Z-Near##PerspectiveCamera", &camera->GetZNear_NonConst(), 0.1f, 100.0f);
        ImGui::SliderFloat("Z-Far##PerspectiveCamera", &camera->GetZFar_NonConst(), 1000.0f, 1000000.0f);
    }
}

void Canavar::Engine::Gui::Draw(FreeCamera* node)
{
    if (!ImGui::CollapsingHeader("Speed##Free Camera"))
    {
        ImGui::SliderFloat("Linear Speed##FreeCamera", &node->GetSpeed_NonConst().mLinear, 0.1f, 10000.0f);
        ImGui::SliderFloat("Linear Speed Multiplier##FreeCamera", &node->GetSpeed_NonConst().mLinearMultiplier, 0.1f, 10.0f);
        ImGui::SliderFloat("Angular Speed##FreeCamera", &node->GetSpeed_NonConst().mAngular, 0.1f, 100.0f);
        ImGui::SliderFloat("Angular Speed Multiplier##FreeCamera", &node->GetSpeed_NonConst().mAngularMultiplier, 0.1f, 10.0f);
    }
}

void Canavar::Engine::Gui::Draw(Terrain* node)
{
    if (!ImGui::CollapsingHeader("Terrain"))
    {
        ImGui::SliderFloat("Amplitude##Terrain", &node->GetAmplitude_NonConst(), 0.0f, 50.0f, "%.3f");
        ImGui::SliderInt("Octaves##Terrain", &node->GetOctaves_NonConst(), 1, 20);
        ImGui::SliderFloat("Power##Terrain", &node->GetPower_NonConst(), 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Tessellation Multiplier##Terrain", &node->GetTessellationMultiplier_NonConst(), 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Grass Coverage##Terrain", &node->GetGrassCoverage_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Ambient##Terrain", &node->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Terrain", &node->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Terrain", &node->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess##Terrain", &node->GetShininess_NonConst(), 0.1f, 128.0f, "%.3f");
        ImGui::Checkbox("Enabled", &node->GetEnabled_NonConst());

        if (ImGui::Button("Generate Seed##Terrain"))
            node->SetSeed(Canavar::Engine::Helper::GenerateVec3(1, 1, 1));

        if (ImGui::Button("Reset##Terrain"))
            node->Reset();
    }
}

void Canavar::Engine::Gui::Draw(Light* node)
{
    if (!ImGui::CollapsingHeader("Shading Parameters##Light"))
    {
        ImGui::SliderFloat("Ambient##Light", &node->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Light", &node->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Light", &node->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::ColorEdit4("Color##Light", (float*)&node->GetColor_NonConst());
    }
}

void Canavar::Engine::Gui::Draw(PointLight* node)
{
    if (!ImGui::CollapsingHeader("Attenuation##PointLight"))
    {
        ImGui::SliderFloat("Constant##PointLight", &node->GetConstant_NonConst(), 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("Linear##PointLight", &node->GetLinear_NonConst(), 0.0f, 0.1f, "%.3f");
        ImGui::SliderFloat("Quadratic##PointLight", &node->GetQuadratic_NonConst(), 0.00001f, 0.001f, "%.6f");
    }
}

void Canavar::Engine::Gui::Draw(Haze* node)
{
    if (!ImGui::CollapsingHeader("Haze##Haze"))
    {
        ImGui::SliderFloat("Density##Haze", &node->GetDensity_NonConst(), 0.0f, 4.0f, "%.3f");
        ImGui::SliderFloat("Gradient##Haze", &node->GetGradient_NonConst(), 0.0f, 4.0f, "%.3f");
        ImGui::ColorEdit4("Color##Haze", (float*)&node->GetColor_NonConst());
    }
}

void Canavar::Engine::Gui::Draw(NozzleEffect* node)
{
    if (!ImGui::CollapsingHeader("Nozzle Effect Parameters##NozzleEffect"))
    {
        ImGui::SliderFloat("Max Radius##NozzleEffect", &node->GetMaxRadius_NonConst(), 0.001f, 4.0f, "%.4f");
        ImGui::SliderFloat("Max Life##NozzleEffect", &node->GetMaxLife_NonConst(), 0.0000f, 0.1f, "%.5f");
        ImGui::SliderFloat("Max Distance##NozzleEffect", &node->GetMaxDistance_NonConst(), 1.0f, 30.0f, "%.3f");
        ImGui::SliderFloat("Min Distance##NozzleEffect", &node->GetMinDistance_NonConst(), 1.0f, 30.0f, "%.3f");
        ImGui::SliderFloat("Speed##NozzleEffect", &node->GetSpeed_NonConst(), 0.0f, 10.0f, "%.5f");
        ImGui::SliderFloat("Scale##NozzleEffect", &node->GetScale_NonConst(), 0.001f, 0.1f, "%.4f");
    }
}

void Canavar::Engine::Gui::Draw(FirecrackerEffect* node)
{
    if (!ImGui::CollapsingHeader("Firecracker Effect Parameters##Firecracker"))
    {
        ImGui::SliderFloat("Span Angle##Firecracker", &node->GetSpanAngle_NonConst(), 0.0f, 180.0f, "%.1f");
        ImGui::SliderFloat("Gravity##Firecracker", &node->GetGravity_NonConst(), 0.0f, 100.0f, "%.1f");
        ImGui::SliderFloat("Max Life##Firecracker", &node->GetMaxLife_NonConst(), 0.0f, 20.0f, "%.2f");
        ImGui::SliderFloat("Min Life##Firecracker", &node->GetMinLife_NonConst(), 0.0f, 20.0f, "%.2f");
        ImGui::SliderFloat("Initial Speed##Firecracker", &node->GetInitialSpeed_NonConst(), 0.0f, 2000.0f, "%.1f");
        ImGui::SliderFloat("Damping##Firecracker", &node->GetDamping_NonConst(), 0.000f, 10.0f, "%.2f");
        ImGui::SliderFloat("Scale##Firecracker", &node->GetScale_NonConst(), 0.001f, 10.0f, "%.4f");
        ImGui::Checkbox("Loop##Firecracker", &node->GetLoop_NonConst());
    }
}

void Canavar::Engine::Gui::Draw(Canavar::Engine::LightningStrikeGenerator* node)
{
    if (!ImGui::CollapsingHeader("Lightning Strike Generator##LightningStrikeGenerator"))
    {
        ImGui::SliderFloat("Base Value##LightningStrikeGenerator", &node->GetBaseValue_NonConst(), 0.01f, 2.0f, "%.2f");
        ImGui::SliderFloat("Decay##LightningStrikeGenerator", &node->GetDecay_NonConst(), 0.1f, 2.0f, "%.3f");
        ImGui::SliderFloat("Jitter Displacement Multiplier##LightningStrikeGenerator", &node->GetJitterDisplacementMultiplier_NonConst(), 0.01f, 2.0f, "%.2f");
        ImGui::SliderFloat("Fork Length Multiplier##LightningStrikeGenerator", &node->GetForkLengthMultiplier_NonConst(), 0.01f, 2.0f, "%.2f");
        ImGui::SliderFloat("Quad Width##LightningStrikeGenerator", &node->GetQuadWidth_NonConst(), 0.01f, 0.1f, "%.4f");
        ImGui::SliderInt("Subdivision Level##LightningStrikeGenerator", &node->GetSubdivisionLevel_NonConst(), 1, 8);
        ImGui::Checkbox("Freeze", &node->GetFreeze_NonConst());
    }
}

void Canavar::Engine::Gui::Draw(Canavar::Engine::LightningStrikeAttractor* node)
{

    if (!ImGui::CollapsingHeader("Lightning Strike Attractor##LightningStrikeAttractor"))
    {
        if (ImGui::BeginCombo("Assign to a generator", "-"))
        {
            const auto Nodes = mNodeManager->GetNodes();

            for (int i = 0; i < Nodes.size(); ++i)
            {
                if (auto Generator = dynamic_cast<LightningStrikeGenerator*>(Nodes[i]))
                {
                    if (ImGui::Selectable(Generator->GetName().toStdString().c_str()))
                    {
                        Generator->AddAttractor(node);
                    }
                }

            }

            ImGui::EndCombo();
        }
    }
}

void Canavar::Engine::Gui::Draw(Canavar::Engine::LightningStrikeSpherical* node)
{
    if (!ImGui::CollapsingHeader("Lightning Strike Spherical##LightningStrikeSpherical"))
    {
        int NumberOfContactPoints = node->GetNumberOfContactPoints();

        if (ImGui::SliderInt("Number Of Contact Points##LightningStrikeSpherical", &NumberOfContactPoints, 1, 16))
        {
            node->SetNumberOfContactPoints(NumberOfContactPoints);
        }

        ImGui::SliderFloat("Radius##LightningStrikeSpherical", &node->GetRadius_NonConst(), 0.1f, 20.0f, "%.3f");
    }
}

Canavar::Engine::Node* Canavar::Engine::Gui::GetSelectedNode() const
{
    return mSelectedNode;
}

void Canavar::Engine::Gui::SetSelectedNode(Canavar::Engine::Node* newSelectedNode)
{
    SetSelectedMesh(nullptr);
    mSelectedModel = dynamic_cast<Model*>(newSelectedNode);

    if (mSelectedNode)
    {
        mSelectedNode->disconnect(this);

        if (mDrawAllBBs)
            RendererManager::Instance()->AddSelectableNode(mSelectedNode, QVector4D(1, 1, 1, 1));
        else
            RendererManager::Instance()->RemoveSelectableNode(mSelectedNode);
    }

    mSelectedNode = newSelectedNode;

    if (mSelectedNode)
    {
        if (mNodeSelectionEnabled)
            RendererManager::Instance()->AddSelectableNode(mSelectedNode, QVector4D(1, 0, 0, 1));

        connect(mSelectedNode, &QObject::destroyed, this, [=]() { SetSelectedNode(nullptr); });
    }
}

Canavar::Engine::Mesh* Canavar::Engine::Gui::GetSelectedMesh() const
{
    return mSelectedMesh;
}

void Canavar::Engine::Gui::SetSelectedMesh(Canavar::Engine::Mesh* newSelectedMesh)
{
    if (mSelectedMesh)
        RendererManager::Instance()->RemoveSelectedMesh(mSelectedModel);

    mSelectedMesh = newSelectedMesh;

    if (mSelectedMesh)
    {
        SelectedMeshParameters parameters;
        parameters.mMesh = mSelectedMesh;

        RendererManager::Instance()->AddSelectedMesh(mSelectedModel, parameters);
    }
}

int Canavar::Engine::Gui::GetSelectedVertexIndex() const
{
    return mSelectedVertexIndex;
}

void Canavar::Engine::Gui::SetSelectedVertexIndex(int newSelectedVertexIndex)
{
    mSelectedVertexIndex = newSelectedVertexIndex;

    if (mSelectedModel)
        RendererManager::Instance()->GetSelectedMeshParameters_Ref(mSelectedModel).mSelectedVertexID = mSelectedVertexIndex;
}

void Canavar::Engine::Gui::MousePressed(QMouseEvent* event)
{
    auto info = SelectableNodeRenderer::Instance()->GetNodeInfoByScreenPosition(event->position().x(), event->position().y());
    mMouse.mPressedButton = NodeManager::Instance()->GetNodeByID(info.nodeID) == mSelectedNode ? event->button() : Qt::NoButton;
    mMouse.mX = event->position().x();
    mMouse.mY = event->position().y();

    if (event->button() != Qt::LeftButton)
        return;

    if (mVertexSelectionEnabled)
    {
        auto info = SelectableNodeRenderer::Instance()->GetVertexInfoByScreenPosition(event->position().x(), event->position().y());

        if (info.success)
            if (mSelectedModel == NodeManager::Instance()->GetNodeByID(info.nodeID))
                if (auto data = ModelDataManager::Instance()->GetModelData(mSelectedModel->GetModelName()))
                    if (data->GetMeshByID(info.meshID) == mSelectedMesh)
                        SetSelectedVertexIndex(info.vertexID);
    }
    else if (mMeshSelectionEnabled)
    {
        auto info = SelectableNodeRenderer::Instance()->GetNodeInfoByScreenPosition(event->position().x(), event->position().y());

        if (info.success)
            if (mSelectedModel == NodeManager::Instance()->GetNodeByID(info.nodeID))
                if (auto data = ModelDataManager::Instance()->GetModelData(mSelectedModel->GetModelName()))
                    SetSelectedMesh(data->GetMeshByID(info.meshID));
    }
    else if (mNodeSelectionEnabled && !mLockNode)
    {
        auto info = SelectableNodeRenderer::Instance()->GetNodeInfoByScreenPosition(event->position().x(), event->position().y());

        if (info.success)
            SetSelectedNode(NodeManager::Instance()->GetNodeByID(info.nodeID));
        else
            SetSelectedNode(nullptr);
    }
}

void Canavar::Engine::Gui::MouseMoved(QMouseEvent* event)
{
    if (mMoveEnabled && mNodeSelectionEnabled && mSelectedNode && mMouse.mPressedButton == Qt::LeftButton)
    {
        auto vp = CameraManager::Instance()->GetActiveCamera()->GetViewProjectionMatrix();
        auto ssc = vp * QVector4D(mSelectedNode->WorldPosition(), 1);

        QVector4D prevMouseWorldPos;
        QVector4D currMouseWorldPos;

        {
            float x = (mMouse.mX - 0.5f * mWidth) / (0.5f * mWidth);
            float y = (0.5f * mHeight - mMouse.mY) / (0.5f * mHeight);
            float z = ssc.z();
            float w = ssc.w();
            prevMouseWorldPos = vp.inverted() * QVector4D(x * w, y * w, z, w);

            mMouse.mX = event->position().x();
            mMouse.mY = event->position().y();
        }

        {
            float x = (event->position().x() - 0.5f * mWidth) / (0.5f * mWidth);
            float y = (0.5f * mHeight - event->position().y()) / (0.5f * mHeight);
            float z = ssc.z();
            float w = ssc.w();
            currMouseWorldPos = vp.inverted() * QVector4D(x * w, y * w, z, w);
        }

        auto delta = (currMouseWorldPos - prevMouseWorldPos).toVector3D();

        QVector3D newWorldPos = mSelectedNode->WorldPosition();

        if (mLockAxisX)
            newWorldPos += QVector3D(delta.x(), 0, 0);

        if (mLockAxisY)
            newWorldPos += QVector3D(0, delta.y(), 0);

        if (mLockAxisZ)
            newWorldPos += QVector3D(0, 0, delta.z());

        if (!mLockAxisX && !mLockAxisY && !mLockAxisZ)
            newWorldPos += delta;

        mSelectedNode->SetWorldPosition(newWorldPos);
    }
}

void Canavar::Engine::Gui::MouseReleased(QMouseEvent* event)
{
    mMouse.mPressedButton = Qt::NoButton;
}

void Canavar::Engine::Gui::KeyPressed(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete)
    {
        if (mSelectedNode)
        {
            NodeManager::Instance()->RemoveNode(mSelectedNode);
            mMeshSelectionEnabled = false;
            mVertexSelectionEnabled = false;
        }
    }
}

void Canavar::Engine::Gui::Resize(int w, int h)
{
    mWidth = w;
    mHeight = h;
}