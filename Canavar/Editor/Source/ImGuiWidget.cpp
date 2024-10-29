#include "ImGuiWidget.h"

#include "Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Light/PointLight.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"
#include "Canavar/Engine/Util/Util.h"

#include <imgui.h>

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
    ImGui::Begin("Debug");
    DrawSky();
    DrawSun();
    DrawHaze();
    DrawTerrain();
    DrawRenderSettings();
    DrawNodeInfo();
    DrawStats();
    ImGui::End();

    DrawObjectsWindow();

    DrawWorldPositionsWindow();
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

void Canavar::Editor::ImGuiWidget::DrawSky()
{
    if (ImGui::CollapsingHeader("Sky##DrawSky"))
    {
        ImGui::SliderFloat("Albedo##Sky", &mSky->GetAlbedo_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Turbidity##Sky", &mSky->GetTurbidity_NonConst(), 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("Normalized Sun Y##Sun", &mSky->GetNormalizedSunY_NonConst(), 0.0f, 10.0f, "%.3f");
        ImGui::Checkbox("Enabled", &mSky->GetEnabled_NonConst());
    }
}

void Canavar::Editor::ImGuiWidget::DrawSun()
{
    if (ImGui::CollapsingHeader("Sun##Sun"))
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
}

void Canavar::Editor::ImGuiWidget::DrawTerrain()
{
    if (ImGui::CollapsingHeader("Terrain##DrawTerrain"))
    {
        ImGui::SliderFloat("Amplitude##Terrain", &mTerrain->GetAmplitude_NonConst(), 0.0f, 50.0f, "%.3f");
        ImGui::SliderInt("Octaves##Terrain", &mTerrain->GetOctaves_NonConst(), 1, 20);
        ImGui::SliderFloat("Power##Terrain", &mTerrain->GetPower_NonConst(), 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Tessellation Multiplier##Terrain", &mTerrain->GetTessellationMultiplier_NonConst(), 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Grass Coverage##Terrain", &mTerrain->GetGrassCoverage_NonConst(), 0.0f, 1.0f, "%.3f");
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
}

void Canavar::Editor::ImGuiWidget::DrawHaze()
{
    if (ImGui::CollapsingHeader("Haze##DrawHaze"))
    {
        ImGui::SliderFloat("Density##Haze", &mHaze->GetDensity_NonConst(), 0.0f, 4.0f, "%.3f");
        ImGui::SliderFloat("Gradient##Haze", &mHaze->GetGradient_NonConst(), 0.0f, 4.0f, "%.3f");
        ImGui::ColorEdit4("Color##Haze", (float *) &mHaze->GetColor_NonConst());
        ImGui::Checkbox("Enabled##Haze", &mHaze->GetEnabled_NonConst());
    }
}

void Canavar::Editor::ImGuiWidget::DrawRenderSettings()
{
    if (ImGui::CollapsingHeader("Render Settings"))
    {
        ImGui::SliderInt("Blur Pass", &mRenderingManager->GetBlurPass_NonConst(), 0, 24);
        ImGui::Checkbox("Draw Bounding Boxes", &mRenderingManager->GetDrawBoundingBoxes_NonConst());
    }
}

void Canavar::Editor::ImGuiWidget::DrawNodeInfo()
{
    if (ImGui::CollapsingHeader("Node Information"))
    {
        if (mNodeInfo.success)
        {
            if (const auto pNode = mNodeManager->GetNodeById(mNodeInfo.nodeID))
            {
                ImGui::Text("Node Name:  %s", pNode->GetNodeName().toStdString().c_str());
                ImGui::Text("Node ID:    %u", pNode->GetNodeId());

                if (const auto pModel = std::dynamic_pointer_cast<Engine::Model>(pNode))
                {
                    if (const auto pMesh = mNodeManager->GetMeshById(pModel, mNodeInfo.meshID))
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

void Canavar::Editor::ImGuiWidget::DrawWorldPositionsWindow()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Saved World Positions");

    ImGui::TextWrapped("Right-click on a fragment adds its world position to this list.");
    ImGui::Spacing();
    ImGui::TextWrapped("You can then assign this position to an object in 'Objects Tree View' window.");
    ImGui::Spacing();

    if (mSavedWorldPositions.size() > 0)
    {
        ImGui::BeginListBox(" ");

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

void Canavar::Editor::ImGuiWidget::DrawObjectsWindow()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Objects");

    if (ImGui::CollapsingHeader("Tree View", ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawObjectsTreeView();
    }

    ImGui::BeginDisabled(mSelectedObject == nullptr);
    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawObject(mSelectedObject);
    }
    ImGui::EndDisabled();

    ImGui::End();
}

void Canavar::Editor::ImGuiWidget::DrawObjectsTreeView()
{
    static ImGuiTableFlags FLAGS = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_RowBg;

    if (ImGui::BeginTable("Objects", 3, FLAGS))
    {
        // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
        ImGui::TableSetupColumn("Object Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Node ID", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        struct CanavarTreeNode
        {
            static void DisplayNode(Engine::ObjectPtr pObject, Engine::ObjectPtr &pSelectedObject)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                if (pObject->GetChildren().size() > 0)
                {
                    bool open = ImGui::TreeNodeEx(pObject->GetUniqueNodeName().toStdString().c_str()); // Column 1
                    ImGui::TableNextColumn();
                    ImGui::Text("%u", pObject->GetNodeId()); // Column 2
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(std::format("##{}", pObject->GetNodeId()).c_str(), pSelectedObject == pObject)) // Column 3
                    {
                        pSelectedObject = pObject;
                    }

                    if (open)
                    {
                        const auto &children = pObject->GetChildren();

                        for (const auto &pChild : children)
                        {
                            CanavarTreeNode::DisplayNode(pChild, pSelectedObject); // Recursive call
                        }

                        ImGui::TreePop();
                    }
                }
                else
                {
                    ImGui::TreeNodeEx(pObject->GetUniqueNodeName().toStdString().c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen); // Column 1
                    ImGui::TableNextColumn();
                    ImGui::Text("%u", pObject->GetNodeId()); // Column 2
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(std::format("##{}", pObject->GetNodeId()).c_str(), pSelectedObject == pObject)) // Column 3
                    {
                        pSelectedObject = pObject;
                    }
                }
            }
        };

        const auto &objects = mNodeManager->GetObjects();

        for (const auto &pObject : objects)
        {
            if (pObject->GetParent() == nullptr) // Call only root objects
            {
                CanavarTreeNode::DisplayNode(pObject, mSelectedObject);
            }
        }

        ImGui::EndTable();
    }
}

void Canavar::Editor::ImGuiWidget::DrawObject(Engine::ObjectPtr pObject)
{
    if (pObject)
    {
        ImGui::Text("Node Name: %s", pObject->GetNodeName().toStdString().c_str());
        ImGui::Text("Node ID:   %u", pObject->GetNodeId());

        ImGui::Spacing();

        auto WorldPosition = pObject->GetWorldPosition();
        if (ImGui::InputFloat3("World Position##DrawObject", &WorldPosition[0], "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
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
        if (ImGui::InputFloat3("Position##DrawObject", &LocalPosition[0], "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
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
        if (ImGui::InputFloat3("Scale##DrawObject", &Scale[0], "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            pObject->SetScale(Scale);
        }

        auto ScaleMin = std::min(Scale.x(), std::min(Scale.y(), Scale.z()));
        if (ImGui::DragFloat("Scale##DragFloatObjectScale", &ScaleMin, 0.01f, 0.001, 100.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            pObject->SetScale(ScaleMin, ScaleMin, ScaleMin);
        }

        if (ImGui::Button("Go to node"))
        {
            emit GoToObject(pObject);
        }
    }

    if (Engine::ModelPtr pModel = std::dynamic_pointer_cast<Engine::Model>(pObject))
    {
        ImGui::Text("Model Parameters");

        ImGui::SliderFloat("Ambient##Model", &pModel->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Model", &pModel->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Model", &pModel->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess##Model", &pModel->GetShininess_NonConst(), 1.0f, 128.0f, "%.3f");
        ImGui::ColorEdit4("Color##PointLight", &pModel->GetColor_NonConst()[0]);
    }
    else if (Engine::DirectionalLightPtr pDirectionalLight = std::dynamic_pointer_cast<Engine::DirectionalLight>(pObject))
    {
        ImGui::Text("Directional Light Parameters");

        ImGui::SliderFloat("Ambient##DirectionalLight", &pDirectionalLight->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##DirectionalLight", &pDirectionalLight->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##DirectionalLight", &pDirectionalLight->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::ColorEdit4("Color##DirectionalLight", (float *) &pDirectionalLight->GetColor_NonConst());

        float theta = pDirectionalLight->GetTheta();
        float phi = pDirectionalLight->GetPhi();
        ImGui::SliderFloat("Theta##DirectionalLight", &theta, -179.0f, 179.0f, "%.1f");
        ImGui::SliderFloat("Phi##DirectionalLight", &phi, -89.0f, 89.0f, "%.1f");
        pDirectionalLight->SetDirectionFromThetaPhi(theta, phi);
    }

    else if (Engine::PointLightPtr pPointLight = std::dynamic_pointer_cast<Engine::PointLight>(pObject))
    {
        ImGui::Text("Point Light Parameters");

        ImGui::SliderFloat("Ambient##PointLight", &pPointLight->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##PointLight", &pPointLight->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##PointLight", &pPointLight->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Constant##PointLight", &pPointLight->GetConstant_NonConst(), 0.0f, 3.0f, "%.3f");
        ImGui::SliderFloat("Linear##PointLight", &pPointLight->GetLinear_NonConst(), 0.0f, 0.1f, "%.3f");
        ImGui::SliderFloat("Quadratic##PointLight", &pPointLight->GetQuadratic_NonConst(), 0.00001f, 0.001f, "%.6f");
        ImGui::ColorEdit4("Color##PointLight", (float *) &pPointLight->GetColor_NonConst());
    }
    else if (Engine::NozzleEffectPtr pNozzleEffect = std::dynamic_pointer_cast<Engine::NozzleEffect>(pObject))
    {
        ImGui::Text("Nozzle Effect Parameters");

        ImGui::SliderFloat("Max Radius##NozzleEffect", &pNozzleEffect->GetMaxRadius_NonConst(), 0.001f, 4.0f, "%.4f");
        ImGui::SliderFloat("Max Life##NozzleEffect", &pNozzleEffect->GetMaxLife_NonConst(), 0.0000f, 0.1f, "%.5f");
        ImGui::SliderFloat("Max Distance##NozzleEffect", &pNozzleEffect->GetMaxDistance_NonConst(), 1.0f, 30.0f, "%.3f");
        ImGui::SliderFloat("Min Distance##NozzleEffect", &pNozzleEffect->GetMinDistance_NonConst(), 1.0f, 30.0f, "%.3f");
        ImGui::SliderFloat("Speed##NozzleEffect", &pNozzleEffect->GetSpeed_NonConst(), 0.0f, 10.0f, "%.5f");
        ImGui::SliderFloat("Scale##NozzleEffect", &pNozzleEffect->GetScale_NonConst(), 0.001f, 0.1f, "%.4f");
    }
    else if (Engine::LightningStrikeGeneratorPtr pLightningStrike = std::dynamic_pointer_cast<Engine::LightningStrikeGenerator>(pObject))
    {
        ImGui::Text("Lightning Strike Generator");

        ImGui::SliderFloat("Base Value##LightningStrikeGenerator", &pLightningStrike->GetBaseValue_NonConst(), 0.01f, 2.0f, "%.2f");
        ImGui::SliderFloat("Decay##LightningStrikeGenerator", &pLightningStrike->GetDecay_NonConst(), 0.1f, 2.0f, "%.3f");
        ImGui::SliderFloat("Jitter Displacement Multiplier##LightningStrikeGenerator", &pLightningStrike->GetJitterDisplacementMultiplier_NonConst(), 0.01f, 2.0f, "%.2f");
        ImGui::SliderFloat("Fork Length Multiplier##LightningStrikeGenerator", &pLightningStrike->GetForkLengthMultiplier_NonConst(), 0.01f, 2.0f, "%.2f");
        ImGui::SliderFloat("Quad Width##LightningStrikeGenerator", &pLightningStrike->GetQuadWidth_NonConst(), 0.01f, 0.1f, "%.4f");
        ImGui::SliderInt("Subdivision Level##LightningStrikeGenerator", &pLightningStrike->GetSubdivisionLevel_NonConst(), 1, 8);
        ImGui::Checkbox("Freeze", &pLightningStrike->GetFreeze_NonConst());
    }
}