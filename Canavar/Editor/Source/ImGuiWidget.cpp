#include "ImGuiWidget.h"

#include <Canavar/Engine/Node/Object/DummyObject/DummyObject.h>
#include <Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h>
#include <Canavar/Engine/Node/Object/Light/DirectionalLight.h>
#include <Canavar/Engine/Node/Object/Light/PointLight.h>
#include <Canavar/Engine/Node/Object/LightningStrike/LightningStrikeAttractor.h>
#include <Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h>
#include <Canavar/Engine/Util/Util.h>
#include <imgui.h>

#include <QFileDialog>

void Canavar::Editor::ImGuiWidget::Initialize()
{
    mSky = mNodeManager->GetSky();
    mTerrain = mNodeManager->GetTerrain();
    mHaze = mNodeManager->GetHaze();
    mSun = mNodeManager->GetSun();
}

void Canavar::Editor::ImGuiWidget::Draw()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_MenuBar);

    DrawMenuBar();
    DrawCreateObjectWidget();
    DrawCreateModelWidget();
    DrawRenderSettings();
    DrawVertexPainterSettings();
    DrawNodeInfo();
    DrawStats();

    ImGui::End();

    DrawNodeParametersWindow();
    DrawNodeTreeViewWindow();
    DrawWorldPositionsWindow();
}

void Canavar::Editor::ImGuiWidget::DrawNodeParametersWindow()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Parameters");

    ImGui::BeginDisabled(mSelectedNode == nullptr);
    if (Engine::NodePtr pNode = std::dynamic_pointer_cast<Engine::Node>(mSelectedNode))
    {
        ImGui::Text("Node Type: %s", pNode->GetNodeType().toStdString().c_str());
        ImGui::Text("Node ID:   %u", pNode->GetNodeId());

        if (const auto newNodeName = InputText("Node Name", pNode->GetNodeName().toStdString()))
        {
            pNode->SetNodeName(newNodeName.value().c_str());
        }
    }

    if (mSelectedNode == mSun)
    {
        DrawSun();
    }
    else if (mSelectedNode == mSky)
    {
        DrawSky();
    }
    else if (mSelectedNode == mHaze)
    {
        DrawHaze();
    }
    else if (mSelectedNode == mTerrain)
    {
        DrawTerrain();
    }
    else if (Engine::ObjectPtr pObject = std::dynamic_pointer_cast<Engine::Object>(mSelectedNode))
    {
        DrawObject(pObject);

        if (Engine::ModelPtr pModel = std::dynamic_pointer_cast<Engine::Model>(mSelectedNode))
        {
            DrawModel(pModel);
        }
        else if (Engine::PerspectiveCameraPtr pPerspectiveCamera = std::dynamic_pointer_cast<Engine::PerspectiveCamera>(mSelectedNode))
        {
            DrawCamera(pPerspectiveCamera);
        }
        else if (Engine::DirectionalLightPtr pDirectionalLight = std::dynamic_pointer_cast<Engine::DirectionalLight>(mSelectedNode))
        {
            DrawDirectionalLight(pDirectionalLight);
        }
        else if (Engine::PointLightPtr pPointLight = std::dynamic_pointer_cast<Engine::PointLight>(mSelectedNode))
        {
            DrawPointLight(pPointLight);
        }
        else if (Engine::NozzleEffectPtr pNozzleEffect = std::dynamic_pointer_cast<Engine::NozzleEffect>(mSelectedNode))
        {
            DrawNozzleEffect(pNozzleEffect);
        }
        else if (Engine::LightningStrikeBasePtr pLightingStrike = std::dynamic_pointer_cast<Engine::LightningStrikeBase>(mSelectedNode))
        {
            DrawLightningStrike(pLightingStrike);
        }
    }

    ImGui::EndDisabled();

    ImGui::End();
}

void Canavar::Editor::ImGuiWidget::DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Import"))
            {
                QString path = QFileDialog::getOpenFileName(nullptr, "Import nodes", "", "*.json");

                if (path.isNull() == false)
                {
                    qDebug() << "ImGuiWidget::DrawMenuBar: Path is" << path;
                    mNodeManager->ImportNodes(path);
                }
            }

            if (ImGui::MenuItem("Export"))
            {
                QString path = QFileDialog::getSaveFileName(nullptr, "Export nodes", "", "*.json");

                if (path.isNull() == false)
                {
                    qDebug() << "ImGuiWidget::DrawMenuBar: Path is" << path;
                    mNodeManager->ExportNodes(path);
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void Canavar::Editor::ImGuiWidget::DrawNodeTreeViewWindow()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Node Tree View");

    static ImGuiTableFlags FLAGS = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_RowBg;

    if (ImGui::BeginTable("Nodes", 3, FLAGS))
    {
        // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoHide);
        ImGui::TableHeadersRow();

        struct CanavarTreeNode
        {
            static void DisplayNode(Engine::NodePtr pNode, Engine::NodePtr &pSelectedNode)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                if (const auto pObject = std::dynamic_pointer_cast<Engine::Object>(pNode))
                {
                    if (pObject->GetChildren().size() > 0)
                    {
                        bool open = ImGui::TreeNodeEx(pObject->GetUniqueNodeName().toStdString().c_str()); // Column 1
                        ImGui::TableNextColumn();
                        ImGui::Text(pObject->GetNodeType().toStdString().c_str());
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(std::format("{}", pObject->GetNodeId()).c_str(), pSelectedNode == pObject)) // Column 2
                        {
                            pSelectedNode = pObject;
                        }

                        if (open)
                        {
                            const auto &children = pObject->GetChildren();

                            for (const auto &pChild : children)
                            {
                                CanavarTreeNode::DisplayNode(pChild, pSelectedNode); // Recursive call
                            }

                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        ImGui::TreeNodeEx(pNode->GetUniqueNodeName().toStdString().c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen); // Column 1
                        ImGui::TableNextColumn();
                        ImGui::Text(pNode->GetNodeType().toStdString().c_str());
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(std::format("{}", pNode->GetNodeId()).c_str(), pSelectedNode == pNode)) // Column 2
                        {
                            pSelectedNode = pNode;
                        }
                    }
                }
                else
                {
                    ImGui::TreeNodeEx(pNode->GetUniqueNodeName().toStdString().c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen); // Column 1
                    ImGui::TableNextColumn();
                    ImGui::Text(pNode->GetNodeType().toStdString().c_str());
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(std::format("{}", pNode->GetNodeId()).c_str(), pSelectedNode == pNode)) // Column 2
                    {
                        pSelectedNode = pNode;
                    }
                }
            }
        };

        const auto &nodes = mNodeManager->GetNodes();

        for (const auto &pNode : nodes)
        {
            if (const auto pObject = std::dynamic_pointer_cast<Engine::Object>(pNode))
            {
                if (pObject->GetParent() == nullptr) // Call only root objects, children will be handled by recursion
                {
                    CanavarTreeNode::DisplayNode(pObject, mSelectedNode);
                }
            }
            else
            {
                CanavarTreeNode::DisplayNode(pNode, mSelectedNode);
            }
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

void Canavar::Editor::ImGuiWidget::DrawSky()
{
    ImGui::SliderFloat("Albedo##Sky", &mSky->GetAlbedo_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Turbidity##Sky", &mSky->GetTurbidity_NonConst(), 0.0f, 10.0f, "%.3f");
    ImGui::SliderFloat("Normalized Sun Y##Sun", &mSky->GetNormalizedSunY_NonConst(), 0.0f, 10.0f, "%.3f");
    ImGui::Checkbox("Enabled", &mSky->GetEnabled_NonConst());
}

void Canavar::Editor::ImGuiWidget::DrawSun()
{
    ImGui::SliderFloat("Ambient##Sun", &mSun->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##Sun", &mSun->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Specular##Sun", &mSun->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");

    float theta = mSun->GetTheta();
    float phi = mSun->GetPhi();
    ImGui::SliderFloat("Theta##Sun", &theta, -179.0f, 179.0f, "%.1f");
    ImGui::SliderFloat("Phi##Sun", &phi, -89.0f, 89.0f, "%.1f");
    mSun->SetDirectionFromThetaPhi(theta, phi);

    ImGui::ColorEdit4("Color##Sun", (float *) &mSun->GetColor_NonConst());
}

void Canavar::Editor::ImGuiWidget::DrawTerrain()
{
    ImGui::SliderFloat("Amplitude##Terrain", &mTerrain->GetAmplitude_NonConst(), 0.0f, 50.0f, "%.3f");
    ImGui::SliderInt("Octaves##Terrain", &mTerrain->GetOctaves_NonConst(), 1, 20);
    ImGui::SliderFloat("Power##Terrain", &mTerrain->GetPower_NonConst(), 0.1f, 10.0f, "%.3f");
    ImGui::SliderFloat("Tessellation Multiplier##Terrain", &mTerrain->GetTessellationMultiplier_NonConst(), 0.1f, 10.0f, "%.3f");
    ImGui::SliderFloat("Grass Coverage##Terrain", &mTerrain->GetGrassCoverage_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Water Height##Terrain", &mTerrain->GetWaterHeight_NonConst(), -1000.0f, 1000.0f, "%.3f");
    ImGui::SliderFloat("Ambient##Terrain", &mTerrain->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##Terrain", &mTerrain->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Specular##Terrain", &mTerrain->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Shininess##Terrain", &mTerrain->GetShininess_NonConst(), 0.1f, 128.0f, "%.3f");
    ImGui::Checkbox("Enabled", &mTerrain->GetEnabled_NonConst());

    if (ImGui::Button("Generate Seed##Terrain"))
    {
        mTerrain->SetSeed(Canavar::Engine::Util::GenerateRandomVector(1, 1, 1));
    }

    if (ImGui::Button("Reset##Terrain"))
    {
        mTerrain->Reset();
    }
}

void Canavar::Editor::ImGuiWidget::DrawHaze()
{
    ImGui::SliderFloat("Density##Haze", &mHaze->GetDensity_NonConst(), 0.0f, 4.0f, "%.3f");
    ImGui::SliderFloat("Gradient##Haze", &mHaze->GetGradient_NonConst(), 0.0f, 4.0f, "%.3f");
    ImGui::ColorEdit4("Color##Haze", (float *) &mHaze->GetColor_NonConst());
    ImGui::Checkbox("Enabled##Haze", &mHaze->GetEnabled_NonConst());
}

void Canavar::Editor::ImGuiWidget::DrawObject(Engine::ObjectPtr pObject)
{
    const auto pParent = pObject->GetParent();
    const auto &objects = mNodeManager->GetObjects();

    if (ImGui::BeginCombo("Parent", pParent ? pParent->GetNodeName().toStdString().c_str() : "-"))
    {
        for (const auto &pParentCandidate : objects)
        {
            if (pParentCandidate == pObject)
            {
                continue;
            }

            if (ImGui::Selectable(pParentCandidate->GetNodeName().toStdString().c_str(), pParentCandidate == pParent))
            {
                pObject->SetParent(pParentCandidate);
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::Button("Remove Parent##DrawObject"))
    {
        pObject->RemoveParent();
    }

    auto WorldPosition = pObject->GetWorldPosition();
    if (ImGui::DragFloat3("World Position##DrawObject", &WorldPosition[0], 0.1f))
    {
        pObject->SetWorldPosition(WorldPosition);
    }

    ImGui::TextWrapped("You can select a world position in the 'Saved World Positions' list and assign it to this object.");

    ImGui::BeginDisabled(mSelectedWorldPositionIndex == -1);
    if (ImGui::Button("Assign"))
    {
        pObject->SetWorldPosition(mSavedWorldPositions[mSelectedWorldPositionIndex]);
    }
    ImGui::EndDisabled();

    auto LocalPosition = pObject->GetPosition();
    if (ImGui::DragFloat3("Position##DrawObject", &LocalPosition[0], 0.1f))
    {
        pObject->SetPosition(LocalPosition);
    }

    if (ImGui::SliderFloat("Yaw##DrawObject", &pObject->GetYaw(), 0.0f, 360.0f, "%.3f"))
        pObject->SetYaw(pObject->GetYaw());
    if (ImGui::SliderFloat("Pitch##DrawObject", &pObject->GetPitch(), -89.999f, 89.999f, "%.3f"))
        pObject->SetPitch(pObject->GetPitch());
    if (ImGui::SliderFloat("Roll##DrawObject", &pObject->GetRoll(), -179.999f, 179.999f, "%.3f"))
        pObject->SetRoll(pObject->GetRoll());

    auto Scale = pObject->GetScale();
    if (ImGui::DragFloat3("Scale##DrawObject", &Scale[0], 0.001f))
    {
        pObject->SetScale(Scale);
    }

    if (ImGui::Button("Go to node"))
    {
        emit GoToObject(pObject);
    }

    if (ImGui::Button("Remove Object"))
    {
        mNodeManager->RemoveNode(pObject);
        if (pObject == mSelectedNode)
        {
            mSelectedNode = nullptr;
            pObject = nullptr;
        }
    }
}

void Canavar::Editor::ImGuiWidget::DrawModel(Engine::ModelPtr pModel)
{
    ImGui::Text("Model Parameters");

    ImGui::SliderFloat("Ambient##Model", &pModel->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##Model", &pModel->GetDiffuse_NonConst(), 0.0f, 2.0f, "%.3f");
    ImGui::SliderFloat("Specular##Model", &pModel->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Shininess##Model", &pModel->GetShininess_NonConst(), 1.0f, 128.0f, "%.3f");
    ImGui::ColorEdit4("Color##Model", &pModel->GetColor_NonConst()[0]);
    ImGui::Checkbox("Invert Normals##Model", &pModel->GetInvertNormals_NonConst());
    ImGui::Checkbox("Visible##Model", &pModel->GetVisible_NonConst());
}

void Canavar::Editor::ImGuiWidget::DrawCamera(Engine::PerspectiveCameraPtr pCamera)
{
    ImGui::Text("Perspective Camera Parameters");

    ImGui::SliderFloat("Vertical FOV", &pCamera->GetVerticalFov_NonConst(), 1.0f, 179.0f, "%.2f");
    ImGui::InputFloat("Z-Far", &pCamera->GetZFar_NonConst(), 0, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::InputFloat("Z-Near", &pCamera->GetZFar_NonConst(), 0, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
}

void Canavar::Editor::ImGuiWidget::DrawDirectionalLight(Engine::DirectionalLightPtr pLight)
{
    ImGui::Text("Directional Light Parameters");

    ImGui::SliderFloat("Ambient##DirectionalLight", &pLight->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##DirectionalLight", &pLight->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Specular##DirectionalLight", &pLight->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::ColorEdit4("Color##DirectionalLight", (float *) &pLight->GetColor_NonConst());

    float theta = pLight->GetTheta();
    float phi = pLight->GetPhi();
    ImGui::SliderFloat("Theta##DirectionalLight", &theta, -179.0f, 179.0f, "%.1f");
    ImGui::SliderFloat("Phi##DirectionalLight", &phi, -89.0f, 89.0f, "%.1f");
    pLight->SetDirectionFromThetaPhi(theta, phi);
}

void Canavar::Editor::ImGuiWidget::DrawPointLight(Engine::PointLightPtr pLight)
{
    ImGui::Text("Point Light Parameters");

    ImGui::SliderFloat("Ambient##PointLight", &pLight->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##PointLight", &pLight->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Specular##PointLight", &pLight->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Constant##PointLight", &pLight->GetConstant_NonConst(), 0.0f, 3.0f, "%.3f");
    ImGui::SliderFloat("Linear##PointLight", &pLight->GetLinear_NonConst(), 0.0f, 0.1f, "%.3f");
    ImGui::SliderFloat("Quadratic##PointLight", &pLight->GetQuadratic_NonConst(), 0.00001f, 0.001f, "%.6f");
    ImGui::ColorEdit4("Color##PointLight", (float *) &pLight->GetColor_NonConst());
}

void Canavar::Editor::ImGuiWidget::DrawNozzleEffect(Engine::NozzleEffectPtr pNozzleEffect)
{
    ImGui::Text("Nozzle Effect Parameters");

    ImGui::SliderFloat("Max Radius##NozzleEffect", &pNozzleEffect->GetMaxRadius_NonConst(), 0.001f, 4.0f, "%.4f");
    ImGui::SliderFloat("Max Life##NozzleEffect", &pNozzleEffect->GetMaxLife_NonConst(), 0.0000f, 0.1f, "%.5f");
    ImGui::SliderFloat("Max Distance##NozzleEffect", &pNozzleEffect->GetMaxDistance_NonConst(), 1.0f, 30.0f, "%.3f");
    ImGui::SliderFloat("Min Distance##NozzleEffect", &pNozzleEffect->GetMinDistance_NonConst(), 1.0f, 30.0f, "%.3f");
    ImGui::SliderFloat("Speed##NozzleEffect", &pNozzleEffect->GetSpeed_NonConst(), 0.0f, 10.0f, "%.5f");
    ImGui::SliderFloat("Scale##NozzleEffect", &pNozzleEffect->GetScale_NonConst(), 0.001f, 0.1f, "%.4f");
}

void Canavar::Editor::ImGuiWidget::DrawLightningStrike(Engine::LightningStrikeBasePtr pLightningStrike)
{
    ImGui::Text("Lightning Strike Generator");

    ImGui::SliderFloat("Base Value##LightningStrikeGenerator", &pLightningStrike->GetBaseValue_NonConst(), 0.01f, 2.0f, "%.2f");
    ImGui::SliderFloat("Decay##LightningStrikeGenerator", &pLightningStrike->GetDecay_NonConst(), 0.1f, 2.0f, "%.3f");
    ImGui::SliderFloat("Jitter Displacement Multiplier##LightningStrikeGenerator", &pLightningStrike->GetJitterDisplacementMultiplier_NonConst(), 0.01f, 2.0f, "%.2f");
    ImGui::SliderFloat("Fork Length Multiplier##LightningStrikeGenerator", &pLightningStrike->GetForkLengthMultiplier_NonConst(), 0.01f, 10.0f, "%.2f");
    ImGui::SliderInt("Subdivision Level##LightningStrikeGenerator", &pLightningStrike->GetSubdivisionLevel_NonConst(), 1, 8);
    ImGui::Checkbox("Freeze", &pLightningStrike->GetFreeze_NonConst());
}

void Canavar::Editor::ImGuiWidget::DrawRenderSettings()
{
    if (ImGui::CollapsingHeader("Render Settings"))
    {
        ImGui::SliderInt("Blur Pass", &mRenderingManager->GetBlurPass_NonConst(), 0, 24);
        ImGui::Checkbox("Draw Bounding Boxes", &mRenderingManager->GetDrawBoundingBoxes_NonConst());
        ImGui::Checkbox("Shadows", &mRenderingManager->GetShadowsEnabled_NonConst());
        ImGui::SliderFloat("Shadow Bias Coefficent", &mRenderingManager->GetShadowBiasCoefficent_NonConst(), 0.001f, 1.0f, "%.3f");
    }
}

void Canavar::Editor::ImGuiWidget::DrawVertexPainterSettings()
{
    if (ImGui::CollapsingHeader("Vertex Painter Settings"))
    {
        ImGui::Checkbox("Enabled##VertexPainter", &mVertexPainter->GetVertexPaintingEnabled_NonConst());
        ImGui::SliderFloat("Brush Radius##VertexPainter", &mVertexPainter->GetBrushRadius_NonConst(), 0.01f, 2.0f, "%.2f");
        ImGui::ColorEdit4("Brush Color##VertexPainter", (float *) &mVertexPainter->GetBrushColor_NonConst());
    }
}

void Canavar::Editor::ImGuiWidget::DrawCreateObjectWidget()
{
    if (ImGui::CollapsingHeader("Create Object##DrawCreateObjectWidget"))
    {
        if (ImGui::BeginCombo("Create Object##DrawCreateObjectWidgetBeginCombo", mSelectedObjectName.toStdString().c_str()))
        {
            for (const auto &[name, factory] : Engine::Object::OBJECT_FACTORIES)
            {
                if (ImGui::Selectable(name.toStdString().c_str()))
                {
                    mSelectedObjectName = name;
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Create##DrawCreateObjectWidget"))
        {
            if (Engine::Object::OBJECT_FACTORIES.contains(mSelectedObjectName))
            {
                Engine::ObjectPtr pNewObject = Engine::Object::OBJECT_FACTORIES[mSelectedObjectName]();
                pNewObject->SetWorldPosition(GetWorldPositionForCreatedObject());
                mNodeManager->AddNode(pNewObject);
                mSelectedNode = pNewObject;
            }
        }
    }
}

void Canavar::Editor::ImGuiWidget::DrawNodeInfo()
{
    if (ImGui::CollapsingHeader("Node Information"))
    {
        if (mNodeInfo.success)
        {
            if (const auto pNode = mNodeManager->GetNodeById(mNodeInfo.nodeId))
            {
                ImGui::Text("Node Name:  %s", pNode->GetNodeName().toStdString().c_str());
                ImGui::Text("Node ID:    %u", pNode->GetNodeId());

                if (const auto pModel = std::dynamic_pointer_cast<Engine::Model>(pNode))
                {
                    if (const auto pMesh = mNodeManager->GetMeshById(pModel, mNodeInfo.meshId))
                    {
                        ImGui::Text("Mesh Name:  %s", pMesh->GetMeshName().toStdString().c_str());
                        ImGui::Text("Mesh ID:    %u", pMesh->GetMeshId());
                    }
                    else
                    {
                        ImGui::Text("Mesh Name:  -");
                        ImGui::Text("Mesh ID:    -");
                    }
                }
                else
                {
                    ImGui::Text("Mesh Name:  -");
                    ImGui::Text("Mesh ID:    -");
                }
            }
        }
        else
        {
            ImGui::Text("Node Name:  -");
            ImGui::Text("Node ID:    -");
            ImGui::Text("Mesh Name:  -");
            ImGui::Text("Mesh ID:    -");
        }
    }
}

void Canavar::Editor::ImGuiWidget::DrawStats()
{
    if (ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Fragment world position: (%.3f, %.3f, %.3f)", mFragmentWorldPosition.x(), mFragmentWorldPosition.y(), mFragmentWorldPosition.z());
        ImGui::Text("Fragment local position: (%.3f, %.3f, %.3f)", mFragmentLocalPosition.x(), mFragmentLocalPosition.y(), mFragmentLocalPosition.z());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}

void Canavar::Editor::ImGuiWidget::DrawCreateModelWidget()
{
    if (ImGui::CollapsingHeader("Create Model##DrawCreateModelWidget"))
    {
        const auto &scenes = mNodeManager->GetScenes();

        if (ImGui::BeginCombo("Create Model##DrawCreateModelWidgetBeginCombo", (mSelectedSceneName + "##DrawCreateModelWidget").toStdString().c_str()))
        {
            for (const auto &[name, scene] : scenes)
            {
                if (ImGui::Selectable(name.toStdString().c_str()))
                {
                    mSelectedSceneName = name;
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Create##DrawCreateModelWidget"))
        {
            Engine::ModelPtr pModel = std::make_shared<Engine::Model>(mSelectedSceneName);
            pModel->SetWorldPosition(GetWorldPositionForCreatedObject());
            mNodeManager->AddNode(pModel);
            mSelectedNode = pModel;
        }
    }
}

void Canavar::Editor::ImGuiWidget::DrawWorldPositionsWindow()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Saved World Positions");

    ImGui::TextWrapped("Right-click on a fragment adds its world position to this list.");
    ImGui::Spacing();
    ImGui::TextWrapped("You can then assign this position to an object in 'Objects Tree View' window.");
    ImGui::Spacing();

    if (ImGui::BeginListBox(" "))
    {
        for (int i = 0; i < mSavedWorldPositions.size(); ++i)
        {
            const float x = mSavedWorldPositions[i].x();
            const float y = mSavedWorldPositions[i].y();
            const float z = mSavedWorldPositions[i].z();

            const auto text = std::format("{:.6}, {:.6}, {:.6}", x, y, z);

            if (ImGui::Selectable(text.c_str(), i == mSelectedWorldPositionIndex))
            {
                mSelectedWorldPositionIndex = i;
            }
        }

        ImGui::EndListBox();
    }

    ImGui::Spacing();

    if (ImGui::Button("Clear"))
    {
        mSavedWorldPositions.clear();
        mSelectedWorldPositionIndex = -1;
    }

    ImGui::End();
}

QVector3D Canavar::Editor::ImGuiWidget::GetWorldPositionForCreatedObject() const
{
    Engine::CameraPtr pActiveCamera = mCameraManager->GetActiveCamera();
    QVector3D cameraPosition = pActiveCamera->GetWorldPosition();
    QVector3D viewDirection = pActiveCamera->GetViewDirection();

    return cameraPosition + 10 * viewDirection;
}

std::optional<std::string> Canavar::Editor::ImGuiWidget::InputText(const std::string &label, const std::string &text)
{
    char buffer[255];

    strncpy(buffer, text.c_str(), sizeof(buffer) - 1);

    if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        return std::string(buffer);
    }

    return std::nullopt;
}

bool Canavar::Editor::ImGuiWidget::KeyPressed(QKeyEvent *)
{
    return false;
}

bool Canavar::Editor::ImGuiWidget::KeyReleased(QKeyEvent *)
{
    return false;
}

bool Canavar::Editor::ImGuiWidget::MousePressed(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::RightButton)
    {
        mSavedWorldPositions << mFragmentWorldPosition;
        mSelectedWorldPositionIndex = mSavedWorldPositions.lastIndexOf(mFragmentWorldPosition);
        return true;
    }

    return false;
}

bool Canavar::Editor::ImGuiWidget::MouseReleased(QMouseEvent *)
{
    return false;
}

bool Canavar::Editor::ImGuiWidget::MouseMoved(QMouseEvent *pEvent)
{
    const int x = pEvent->position().x();
    const int y = pEvent->position().y();
    mFragmentLocalPosition = mRenderingManager->FetchFragmentLocalPositionFromScreen(x, y);
    mFragmentWorldPosition = mRenderingManager->FetchFragmentWorldPositionFromScreen(x, y);
    mNodeInfo = mRenderingManager->FetchNodeInfoFromScreenCoordinates(x, y);

    return false;
}
