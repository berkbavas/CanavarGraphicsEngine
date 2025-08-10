#include "ImGuiWidget.h"

#include "Canavar/Engine/Node/NodeFactory.h"
#include "Canavar/Engine/Node/Object/DummyObject/DummyObject.h"
#include "Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Light/PointLight.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeAttractor.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"
#include "Canavar/Engine/Util/Util.h"

#include <imgui.h>

#include <QFileDialog>

void Canavar::Engine::ImGuiWidget::Initialize()
{
    mSky = mNodeManager->GetSky();
    mHaze = mNodeManager->GetHaze();
    mSun = mNodeManager->GetSun();
    mTerrain = mNodeManager->GetTerrain();
}

void Canavar::Engine::ImGuiWidget::Draw()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_MenuBar);
    DrawMenuBar();
    DrawNodeTreeViewWidget();
    DrawNodeParametersWidget();
    DrawCreateObjectWidget();
    DrawCreateModelWidget();
    DrawRenderSettings();
    DrawCrossSectionAnalyzerWidget();
    DrawWorldPositionsWidget();
    DrawNodeInfo();
    DrawStats();
    ImGui::End();

    for (auto *pClient : mClients)
    {
        pClient->Draw();
    }
}

void Canavar::Engine::ImGuiWidget::AddClient(ImGuiClient *pClient)
{
    mClients.push_back(pClient);
}

void Canavar::Engine::ImGuiWidget::DrawNodeParametersWidget()
{
    if (ImGui::CollapsingHeader("Parameters"))
    {
        ImGui::BeginDisabled(mSelectedNode == nullptr);
        if (Engine::NodePtr pNode = std::dynamic_pointer_cast<Engine::Node>(mSelectedNode))
        {
            ImGui::Text("Node Type: %s", pNode->GetNodeTypeName());
            ImGui::Text("Node ID:   %u", pNode->GetNodeId());

            if (const auto NewNodeName = InputText("Node Name", pNode->GetNodeName().c_str()))
            {
                pNode->SetNodeName(NewNodeName.value());
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
        else if (Engine::Text2DPtr pText2D = std::dynamic_pointer_cast<Engine::Text2D>(mSelectedNode))
        {
            DrawText2D(pText2D);
        }
        else if (Engine::ObjectPtr pObject = std::dynamic_pointer_cast<Engine::Object>(mSelectedNode))
        {
            DrawObject(pObject);

            if (Engine::ModelPtr pModel = std::dynamic_pointer_cast<Engine::Model>(mSelectedNode))
            {
                DrawModel(pModel);
            }
            else if (Engine::Text3DPtr pText3D = std::dynamic_pointer_cast<Engine::Text3D>(mSelectedNode))
            {
                DrawText3D(pText3D);
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
    }
}

void Canavar::Engine::ImGuiWidget::DrawMenuBar()
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

void Canavar::Engine::ImGuiWidget::DrawNodeTreeViewWidget()
{
    if (ImGui::CollapsingHeader("Node Tree"))
    {
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
                            bool open = ImGui::TreeNodeEx(pObject->GetUniqueNodeName().c_str()); // Column 1
                            ImGui::TableNextColumn();
                            ImGui::Text(pObject->GetNodeTypeName());
                            ImGui::TableNextColumn();
                            if (ImGui::Selectable(pObject->GetNodeIdString().c_str(), pSelectedNode == pObject)) // Column 2
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
                            ImGui::TreeNodeEx(pNode->GetUniqueNodeName().c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen); // Column 1
                            ImGui::TableNextColumn();
                            ImGui::Text(pNode->GetNodeTypeName());
                            ImGui::TableNextColumn();
                            if (ImGui::Selectable(pObject->GetNodeIdString().c_str(), pSelectedNode == pNode)) // Column 2
                            {
                                pSelectedNode = pNode;
                            }
                        }
                    }
                    else
                    {
                        ImGui::TreeNodeEx(pNode->GetUniqueNodeName().c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen); // Column 1
                        ImGui::TableNextColumn();
                        ImGui::Text(pNode->GetNodeTypeName());
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(pNode->GetNodeIdString().c_str(), pSelectedNode == pNode)) // Column 2
                        {
                            pSelectedNode = pNode;
                        }
                    }
                }
            };

            const auto &Nodes = mNodeManager->GetNodes();
            auto pSelectedNode = mSelectedNode;

            for (const auto &pNode : Nodes)
            {
                if (const auto pObject = std::dynamic_pointer_cast<Engine::Object>(pNode))
                {
                    if (pObject->GetParent<Engine::Node>() == nullptr) // Call only root objects, children will be handled by recursion
                    {
                        CanavarTreeNode::DisplayNode(pObject, pSelectedNode);
                    }
                }
                else
                {
                    CanavarTreeNode::DisplayNode(pNode, pSelectedNode);
                }
            }

            if (pSelectedNode != mSelectedNode)
            {
                SetSelectedNode(pSelectedNode);
            }

            ImGui::EndTable();
        }
    }
}

void Canavar::Engine::ImGuiWidget::DrawSky()
{
    ImGui::SliderFloat("Albedo##Sky", &mSky->GetAlbedo_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Turbidity##Sky", &mSky->GetTurbidity_NonConst(), 0.0f, 10.0f, "%.3f");
    ImGui::SliderFloat("Normalized Sun Y##Sun", &mSky->GetNormalizedSunY_NonConst(), 0.0f, 10.0f, "%.3f");
    ImGui::Checkbox("Enabled", &mSky->GetEnabled_NonConst());
}

void Canavar::Engine::ImGuiWidget::DrawSun()
{
    ImGui::SliderFloat("Ambient##Sun", &mSun->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##Sun", &mSun->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Specular##Sun", &mSun->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Radiance##Sun", &mSun->GetRadiance_NonConst(), 0.0f, 100.0f, "%.3f");

    float theta = mSun->GetTheta();
    float phi = mSun->GetPhi();
    ImGui::SliderFloat("Theta##Sun", &theta, -179.0f, 179.0f, "%.1f");
    ImGui::SliderFloat("Phi##Sun", &phi, -89.0f, 89.0f, "%.1f");
    mSun->SetDirectionFromThetaPhi(theta, phi);

    ImGui::ColorEdit4("Color##Sun", (float *) &mSun->GetColor_NonConst());
}

void Canavar::Engine::ImGuiWidget::DrawHaze()
{
    ImGui::SliderFloat("Density##Haze", &mHaze->GetDensity_NonConst(), 0.0f, 4.0f, "%.3f");
    ImGui::SliderFloat("Gradient##Haze", &mHaze->GetGradient_NonConst(), 0.0f, 4.0f, "%.3f");
    ImGui::ColorEdit4("Color##Haze", (float *) &mHaze->GetColor_NonConst());
    ImGui::Checkbox("Enabled##Haze", &mHaze->GetEnabled_NonConst());
}

void Canavar::Engine::ImGuiWidget::DrawTerrain()
{
    ImGui::SliderFloat("Amplitude##Terrain", &mTerrain->GetAmplitude_NonConst(), 0.0f, 50.0f, "%.3f");
    ImGui::SliderInt("Octaves##Terrain", &mTerrain->GetOctaves_NonConst(), 1, 20);
    ImGui::SliderFloat("Frequency##Terrain", &mTerrain->GetFrequency_NonConst(), 0, 1, "%.3f");
    ImGui::SliderFloat("Persistence##Terrain", &mTerrain->GetPersistence_NonConst(), 0, 1, "%.3f");
    ImGui::SliderFloat("Lacunarity##Terrain", &mTerrain->GetLacunarity_NonConst(), 0, 20, "%.3f");
    ImGui::SliderFloat("Tessellation Multiplier##Terrain", &mTerrain->GetTessellationMultiplier_NonConst(), 1, 128, "%.3f");
    ImGui::SliderFloat("Ambient##Terrain", &mTerrain->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##Terrain", &mTerrain->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Specular##Terrain", &mTerrain->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Shininess##Terrain", &mTerrain->GetShininess_NonConst(), 0.1f, 128.0f, "%.3f");
    ImGui::Checkbox("Enabled", &mTerrain->GetEnabled_NonConst());
}

void Canavar::Engine::ImGuiWidget::DrawObject(Engine::ObjectPtr pObject)
{
    const auto pParent = pObject->GetParent<Engine::Node>();
    const auto &Objects = mNodeManager->GetObjects();

    if (ImGui::BeginCombo("Parent", pParent ? pParent->GetNodeName().c_str() : "-"))
    {
        for (const auto &pParentCandidate : Objects)
        {
            if (pParentCandidate == pObject)
            {
                continue;
            }

            if (ImGui::Selectable(pParentCandidate->GetNodeName().c_str(), pParentCandidate == pParent))
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
    if (ImGui::DragFloat3("Position##DrawObject", &LocalPosition[0], 0.01f))
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
        RemoveNode(pObject);
    }
}

void Canavar::Engine::ImGuiWidget::DrawModel(Engine::ModelPtr pModel)
{
    ImGui::Text("Model Parameters");

    if (ImGui::BeginCombo("Shading Mode##DrawModel", pModel->GetShadingMode() == Canavar::Engine::PBR_SHADING ? "Pbr" : "Phong"))
    {
        if (ImGui::Selectable("Pbr"))
        {
            pModel->SetShadingMode(Canavar::Engine::PBR_SHADING);
        }

        if (ImGui::Selectable("Phong"))
        {
            pModel->SetShadingMode(Canavar::Engine::PHONG_SHADING);
        }

        ImGui::EndCombo();
    }

    ImGui::SliderFloat("Ambient##Model", &pModel->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##Model", &pModel->GetDiffuse_NonConst(), 0.0f, 2.0f, "%.3f");
    ImGui::SliderFloat("Specular##Model", &pModel->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Shininess##Model", &pModel->GetShininess_NonConst(), 1.0f, 128.0f, "%.3f");
    ImGui::SliderFloat("Metallic##Model", &pModel->GetMetallic_NonConst(), 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Roughness##Model", &pModel->GetRoughness_NonConst(), 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Ambient Occlusion##Model", &pModel->GetAmbientOcclusion_NonConst(), 0.0f, 10.0f, "%.2f");
    ImGui::Checkbox("Use Model Color##Model", &pModel->GetUseModelColor_NonConst());
    ImGui::ColorEdit3("Color##Model", &pModel->GetColor_NonConst()[0]);
    ImGui::ColorEdit3("Transparency Color##Model", &pModel->GetTransparencyColor_NonConst()[0]);
    ImGui::Checkbox("Visible##Model", &pModel->GetVisible_NonConst());

    ImGui::Text("Mesh Parameters");

    if (ImGui::BeginCombo("Mesh Selector##Model", mSelectedMesh ? mSelectedMesh->GetUniqueMeshName().c_str() : "Select Mesh"))
    {
        const auto pScene = mNodeManager->GetScene(pModel->GetSceneName());
        const auto &Meshes = pScene->GetMeshes();

        for (const auto &pMesh : Meshes)
        {
            if (ImGui::Selectable(pMesh->GetUniqueMeshName().c_str(), mSelectedMesh == pMesh))
            {
                mSelectedMesh = pMesh;
            }
        }

        ImGui::EndCombo();
    }

    if (mSelectedMesh)
    {
        auto nodeId = mSelectedMesh->GetMeshId();
        auto opacity = pModel->GetMeshOpacity(mSelectedMesh->GetUniqueMeshName(), mSelectedMesh->GetOpacity());
        auto visibility = pModel->GetMeshVisibility(mSelectedMesh->GetUniqueMeshName());

        ImGui::Text("Mesh ID: %d", nodeId);
        ImGui::Text("Opacity: %.3f", opacity);

        if (ImGui::Checkbox("Mesh Visible", &visibility))
        {
            pModel->SetMeshVisibility(mSelectedMesh->GetUniqueMeshName(), visibility);
        }

        if (ImGui::SliderFloat("Mesh Opacity", &opacity, 0.0f, 1.0f))
        {
            pModel->SetMeshOpacity(mSelectedMesh->GetUniqueMeshName(), opacity);
        }
    }
}

void Canavar::Engine::ImGuiWidget::DrawText2D(Engine::Text2DPtr pText)
{
    if (const auto newText = InputText("Text##DrawText2D", pText->GetText().toStdString()))
    {
        pText->SetText(newText.value().c_str());
    }
    ImGui::InputFloat2("Position##Text2D", &pText->GetPosition_NonConst()[0]);
    ImGui::SliderFloat("Scale##Text2D", &pText->GetScale_NonConst(), 0.0f, 8.0f, "%.2f");
    ImGui::ColorEdit3("Color##Text2D", &pText->GetColor_NonConst()[0]);
}

void Canavar::Engine::ImGuiWidget::DrawText3D(Engine::Text3DPtr pText)
{
    if (const auto newText = InputText("Text##DrawText3D", pText->GetText().toStdString()))
    {
        pText->SetText(newText.value().c_str());
    }

    ImGui::ColorEdit3("Color##Text3D", &pText->GetColor_NonConst()[0]);
}

void Canavar::Engine::ImGuiWidget::DrawCamera(Engine::PerspectiveCameraPtr pCamera)
{
    ImGui::Text("Perspective Camera Parameters");

    ImGui::SliderFloat("Vertical FOV", &pCamera->GetVerticalFov_NonConst(), 1.0f, 179.0f, "%.2f");
    ImGui::InputFloat("Z-Far", &pCamera->GetZFar_NonConst(), 0, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::InputFloat("Z-Near", &pCamera->GetZNear_NonConst(), 0, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
}

void Canavar::Engine::ImGuiWidget::DrawDirectionalLight(Engine::DirectionalLightPtr pLight)
{
    ImGui::Text("Directional Light Parameters");

    ImGui::SliderFloat("Ambient##DirectionalLight", &pLight->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##DirectionalLight", &pLight->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Specular##DirectionalLight", &pLight->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Radiance##DirectionalLight", &pLight->GetRadiance_NonConst(), 0.0f, 100.0f, "%.3f");
    ImGui::ColorEdit3("Color##DirectionalLight", &pLight->GetColor_NonConst()[0]);

    float theta = pLight->GetTheta();
    float phi = pLight->GetPhi();
    ImGui::SliderFloat("Theta##DirectionalLight", &theta, -179.0f, 179.0f, "%.1f");
    ImGui::SliderFloat("Phi##DirectionalLight", &phi, -89.0f, 89.0f, "%.1f");
    pLight->SetDirectionFromThetaPhi(theta, phi);
}

void Canavar::Engine::ImGuiWidget::DrawPointLight(Engine::PointLightPtr pLight)
{
    ImGui::Text("Point Light Parameters");

    ImGui::SliderFloat("Ambient##PointLight", &pLight->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##PointLight", &pLight->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Specular##PointLight", &pLight->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Constant##PointLight", &pLight->GetConstant_NonConst(), 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Linear##PointLight", &pLight->GetLinear_NonConst(), 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Quadratic##PointLight", &pLight->GetQuadratic_NonConst(), 0.0f, 1.0f, "%.4f");
    ImGui::ColorEdit3("Color##PointLight", (float *) &pLight->GetColor_NonConst());
}

void Canavar::Engine::ImGuiWidget::DrawNozzleEffect(Engine::NozzleEffectPtr pNozzleEffect)
{
    ImGui::Text("Nozzle Effect Parameters");

    ImGui::SliderFloat("Max Radius##NozzleEffect", &pNozzleEffect->GetMaxRadius_NonConst(), 0.0f, 4.0f);
    ImGui::SliderFloat("Max Life##NozzleEffect", &pNozzleEffect->GetMaxLife_NonConst(), 0.00f, 4.0f);
    ImGui::SliderFloat("Max Length##NozzleEffect", &pNozzleEffect->GetMaxLength_NonConst(), 0.0f, 15.0f);
    ImGui::SliderFloat("Max Speed##NozzleEffect", &pNozzleEffect->GetMaxSpeed_NonConst(), 0.0f, 100.0f);
    ImGui::SliderFloat("Power##NozzleEffect", &pNozzleEffect->GetPower_NonConst(), 0.01f, 4.0f);
    ImGui::SliderInt("# of Particles##NozzleEffect", &pNozzleEffect->GetNumberOfParticles_NonConst(), 0, 100'000);
}

void Canavar::Engine::ImGuiWidget::DrawLightningStrike(Engine::LightningStrikeBasePtr pLightningStrike)
{
    ImGui::Text("Lightning Strike Generator");

    ImGui::SliderFloat("Base Value##LightningStrikeGenerator", &pLightningStrike->GetBaseValue_NonConst(), 0.01f, 2.0f, "%.2f");
    ImGui::SliderFloat("Decay##LightningStrikeGenerator", &pLightningStrike->GetDecay_NonConst(), 0.1f, 2.0f, "%.3f");
    ImGui::SliderFloat("Jitter Displacement Multiplier##LightningStrikeGenerator", &pLightningStrike->GetJitterDisplacementMultiplier_NonConst(), 0.01f, 2.0f, "%.2f");
    ImGui::SliderFloat("Fork Length Multiplier##LightningStrikeGenerator", &pLightningStrike->GetForkLengthMultiplier_NonConst(), 0.01f, 10.0f, "%.2f");
    ImGui::SliderInt("Subdivision Level##LightningStrikeGenerator", &pLightningStrike->GetSubdivisionLevel_NonConst(), 1, 8);
    ImGui::Checkbox("Freeze", &pLightningStrike->GetFreeze_NonConst());
}

void Canavar::Engine::ImGuiWidget::DrawRenderSettings()
{
    if (ImGui::CollapsingHeader("Render Settings"))
    {
        const auto pShadowMappingRenderer = mRenderingManager->GetShadowMappingRenderer();
        ImGui::Checkbox("Draw Bounding Boxes", &mRenderingManager->GetDrawBoundingBoxes_NonConst());

        ImGui::Checkbox("Shadows Enabled", &mRenderingManager->GetShadowsEnabled_NonConst());
        ImGui::Checkbox("Use Orthographic Projection", &pShadowMappingRenderer->GetUseOrthographicProjection_NonConst());
        ImGui::SliderFloat("Shadow Bias", &mRenderingManager->GetShadowBias_NonConst(), 0.00001f, 0.001f, "%.5f");
        ImGui::SliderInt("Shadow Samples", &mRenderingManager->GetShadowSamples_NonConst(), 1, 6);

        if (pShadowMappingRenderer->GetUseOrthographicProjection())
        {
            ImGui::SliderFloat("Shadow Orthographic Projection Size", &pShadowMappingRenderer->GetOrthographicSize_NonConst(), 10, 1000);
        }
        else
        {
            ImGui::SliderFloat("Shadow Perspective Projection FOV", &pShadowMappingRenderer->GetFov_NonConst(), 5, 90);
            ImGui::SliderFloat("Shadow Perspective Projection Z-Near", &pShadowMappingRenderer->GetZNear_NonConst(), 1, 20);
            ImGui::SliderFloat("Shadow Perspective Projection Z-Far", &pShadowMappingRenderer->GetZFar_NonConst(), 20, 1000);
        }

        ImGui::SliderFloat("Sun Distance", &pShadowMappingRenderer->GetSunDistance_NonConst(), 1, 1000);

        ImGui::Text("Motion Blur");
        ImGui::Checkbox("Motion Blur Enabled", &mRenderingManager->GetMotionBlurEnabled_NonConst());
        ImGui::SliderInt("Motion Blur Samples", &mRenderingManager->GetMotionBlurSamples_NonConst(), 2, 40);
        ImGui::SliderFloat("Motion Blur Strength", &mRenderingManager->GetMotionBlurStrength_NonConst(), 0.0f, 10.0f, "%.2f");

        ImGui::Text("ACES");
        ImGui::Checkbox("ACES Enabled", &mRenderingManager->GetAcesEnabled_NonConst());
        ImGui::SliderFloat("Exposure", &mRenderingManager->GetExposure_NonConst(), 0.0f, 1.0f, "%.2f");

        ImGui::Text("Cinematic Look");
        ImGui::Checkbox("Cinematic Enabled", &mRenderingManager->GetCinematicEnabled_NonConst());
        ImGui::SliderFloat("Vignette Radius", &mRenderingManager->GetVignetteRadius_NonConst(), 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Vignette Softness", &mRenderingManager->GetVignetteSoftness_NonConst(), 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Grain Strength", &mRenderingManager->GetGrainStrength_NonConst(), 0.0f, 1.0f, "%.2f");

        // God Rays
        // ImGui::Text("God Rays");
        // ImGui::SliderInt("Number of Samples", &mRenderingManager->GetNumberOfSamples_NonConst(), 1, 200);
        // ImGui::SliderFloat("Density", &mRenderingManager->GetDensity_NonConst(), 0.0f, 1.0f, "%.2f");
        // ImGui::SliderFloat("Decay", &mRenderingManager->GetDecay_NonConst(), 0.0f, 1.0f, "%.2f");
        // ImGui::SliderFloat("Weight", &mRenderingManager->GetWeight_NonConst(), 0.0f, 1.0f, "%.2f");
    }
}

void Canavar::Engine::ImGuiWidget::DrawCrossSectionAnalyzerWidget()
{
    if (ImGui::CollapsingHeader("Cross Section Analyzer##DrawCrossSectionAnalyzerWidget"))
    {
        ImGui::ColorEdit4("Plane Color##DrawCrossSectionAnalyzerWidget", (float *) &mRenderingManager->GetCrossSectionAnalyzer()->GetPlaneColor_NonConst());
        ImGui::DragFloat3("Plane Position##DrawCrossSectionAnalyzerWidget", (float *) &mRenderingManager->GetCrossSectionAnalyzer()->GetPlanePosition_NonConst(), 0.005f);
        ImGui::DragFloat3("Plane Scale##DrawCrossSectionAnalyzerWidget", (float *) &mRenderingManager->GetCrossSectionAnalyzer()->GetPlaneScale_NonConst(), 0.005f);

        if (ImGui::Checkbox("Enabled##DrawCrossSectionAnalyzerWidget", &mRenderingManager->GetCrossSectionEnabled_NonConst()))
        {
            if (mRenderingManager->GetCrossSectionEnabled())
            {
                mRenderingManager->GetCrossSectionAnalyzer()->ShowWidget();
            }
            else
            {
                mRenderingManager->GetCrossSectionAnalyzer()->CloseWidget();
            }
        }

        if (ImGui::Button("Assign Position"))
        {
            if (mSelectedWorldPositionIndex != -1)
            {
                mRenderingManager->GetCrossSectionAnalyzer()->SetPlanePosition(mSavedWorldPositions[mSelectedWorldPositionIndex]);
            }
        }
    }
}

void Canavar::Engine::ImGuiWidget::DrawCreateObjectWidget()
{
    if (ImGui::CollapsingHeader("Create Object##DrawCreateObjectWidget"))
    {
        if (ImGui::BeginCombo("Create Object##DrawCreateObjectWidgetBeginCombo", mSelectedObjectName.c_str()))
        {
            for (const auto &name : Engine::NodeFactory::GetNodeNames())
            {
                if (ImGui::Selectable(name.c_str()))
                {
                    mSelectedObjectName = name;
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Create##DrawCreateObjectWidget"))
        {
            if (Engine::NodeFactory::GetNodeNames().contains(mSelectedObjectName))
            {
                Engine::NodePtr pNewNode = Engine::NodeFactory::CreateNode(mSelectedObjectName);
                if (Engine::ObjectPtr pNewObject = std::dynamic_pointer_cast<Engine::Object>(pNewNode))
                {
                    pNewObject->SetWorldPosition(GetWorldPositionForCreatedObject());
                }

                mNodeManager->AddNode(pNewNode);
                SetSelectedNode(pNewNode);
            }
        }
    }
}

void Canavar::Engine::ImGuiWidget::DrawNodeInfo()
{
    if (ImGui::CollapsingHeader("Node Information"))
    {
        if (mNodeInfo.Success)
        {
            if (const auto pNode = mNodeManager->FindNodeById(mNodeInfo.NodeId))
            {
                ImGui::Text("Node Name:  %s", pNode->GetNodeName().c_str());
                ImGui::Text("Node ID:    %u", pNode->GetNodeId());

                if (const auto pModel = std::dynamic_pointer_cast<Engine::Model>(pNode))
                {
                    if (const auto pMesh = mNodeManager->GetMeshById(pModel, mNodeInfo.MeshId))
                    {
                        ImGui::Text("Mesh Name:  %s", pMesh->GetMeshName().c_str());
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

void Canavar::Engine::ImGuiWidget::DrawStats()
{
    if (ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Fragment world position: (%.3f, %.3f, %.3f)", mFragmentWorldPosition.x(), mFragmentWorldPosition.y(), mFragmentWorldPosition.z());
        ImGui::Text("Fragment local position: (%.3f, %.3f, %.3f)", mFragmentLocalPosition.x(), mFragmentLocalPosition.y(), mFragmentLocalPosition.z());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}

void Canavar::Engine::ImGuiWidget::DrawCreateModelWidget()
{
    if (ImGui::CollapsingHeader("Create Model##DrawCreateModelWidget"))
    {
        const auto &Scenes = mNodeManager->GetScenes();

        if (ImGui::BeginCombo("Create Model##DrawCreateModelWidgetBeginCombo", mSelectedSceneName.c_str()))
        {
            for (const auto &[name, scene] : Scenes)
            {
                if (ImGui::Selectable(name.c_str(), mSelectedSceneName == name))
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
            SetSelectedNode(pModel);
        }
    }
}

void Canavar::Engine::ImGuiWidget::DrawWorldPositionsWidget()
{
    if (ImGui::CollapsingHeader("Saved World Positions"))
    {
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
    }
}

QVector3D Canavar::Engine::ImGuiWidget::GetWorldPositionForCreatedObject() const
{
    Engine::CameraPtr pActiveCamera = mCameraManager->GetActiveCamera();
    QVector3D cameraPosition = pActiveCamera->GetWorldPosition();
    QVector3D viewDirection = pActiveCamera->GetViewDirection();

    return cameraPosition + 10 * viewDirection;
}

std::optional<std::string> Canavar::Engine::ImGuiWidget::InputText(const std::string &label, const std::string &text)
{
    static char BUFFER[255];

    strncpy(BUFFER, text.c_str(), sizeof(BUFFER) - 1);

    if (ImGui::InputText(label.c_str(), BUFFER, sizeof(BUFFER), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        return std::string(BUFFER);
    }

    return std::nullopt;
}

bool Canavar::Engine::ImGuiWidget::KeyPressed(QKeyEvent *pEvent)
{
    if (pEvent->key() == Qt::Key_Delete)
    {
        RemoveNode(mSelectedNode);
    }

    return ImGui::GetIO().WantCaptureKeyboard;
}

bool Canavar::Engine::ImGuiWidget::KeyReleased(QKeyEvent *)
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool Canavar::Engine::ImGuiWidget::MousePressed(QMouseEvent *pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return true;
    }
    else if (pEvent->button() == Qt::RightButton)
    {
        mSavedWorldPositions << mFragmentWorldPosition;
        mSelectedWorldPositionIndex = mSavedWorldPositions.lastIndexOf(mFragmentWorldPosition);
        return true;
    }
    else if (pEvent->button() == Qt::LeftButton)
    {
        const int x = pEvent->position().x();
        const int y = pEvent->position().y();
        ProcessMouseAction(x, y);
        if (mNodeInfo.Success)
        {
            const auto pSelectedNode = mNodeManager->FindNodeById(mNodeInfo.NodeId);
            SetSelectedNode(pSelectedNode);
            SetSelectedMesh(pSelectedNode, mNodeInfo.MeshId);
        }
        return true;
    }

    return false;
}

bool Canavar::Engine::ImGuiWidget::MouseReleased(QMouseEvent *)
{
    return ImGui::GetIO().WantCaptureMouse;
}

bool Canavar::Engine::ImGuiWidget::MouseMoved(QMouseEvent *pEvent)
{
    const int x = pEvent->position().x();
    const int y = pEvent->position().y();
    ProcessMouseAction(x, y);
    return ImGui::GetIO().WantCaptureMouse;
}

bool Canavar::Engine::ImGuiWidget::WheelMoved(QWheelEvent *)
{
    return ImGui::GetIO().WantCaptureMouse;
}

void Canavar::Engine::ImGuiWidget::RemoveNode(NodePtr pNode)
{
    if (pNode == nullptr)
    {
        return;
    }

    mNodeManager->RemoveNode(pNode);

    if (pNode == mSelectedNode)
    {
        mSelectedNode = nullptr;
    }
}

void Canavar::Engine::ImGuiWidget::SetSelectedNode(NodePtr pNode)
{
    if (mSelectedNode == pNode)
    {
        return;
    }

    if (ModelPtr pCurrentSelectedModel = std::dynamic_pointer_cast<Model>(mSelectedNode))
    {
        SetSelectedMesh(pCurrentSelectedModel, -1);
    }

    mSelectedNode = pNode;

    if (ModelPtr pCurrentSelectedModel = std::dynamic_pointer_cast<Model>(mSelectedNode))
    {
        SetSelectedMesh(pCurrentSelectedModel, -1);
    }
}

void Canavar::Engine::ImGuiWidget::SetSelectedMesh(NodePtr pNode, uint32_t MeshId)
{
    if (ModelPtr pModel = std::dynamic_pointer_cast<Model>(pNode))
    {
        const auto pScene = mNodeManager->GetScene(pModel->GetSceneName());
        const auto &Meshes = pScene->GetMeshes();

        for (const auto &pMesh : Meshes)
        {
            if (pMesh->GetMeshId() == MeshId)
            {
                mSelectedMesh = pMesh;
                pModel->SetSelectedMeshId(pMesh->GetMeshId());
                return;
            }
        }

        pModel->SetSelectedMeshId(-1);
    }
}

void Canavar::Engine::ImGuiWidget::ProcessMouseAction(int x, int y)
{
    mFragmentLocalPosition = mRenderingManager->FetchFragmentLocalPositionFromScreen(x, y);
    mFragmentWorldPosition = mRenderingManager->FetchFragmentWorldPositionFromScreen(x, y);
    mNodeInfo = mRenderingManager->FetchNodeInfoFromScreenCoordinates(x, y);
}
