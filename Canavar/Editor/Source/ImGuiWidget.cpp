#include "ImGuiWidget.h"

#include "Canavar/Engine/Node/Effects/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Light/PointLight.h"
#include "Canavar/Engine/Node/Model/Model.h"
#include "Canavar/Engine/Util/Math.h"
#include "Canavar/Engine/Util/Util.h"

#include <imgui.h>

void Canavar::Editor::ImGuiWidget::Initialize()
{
    mSun = mNodeManager->GetSun();
    mSky = mNodeManager->GetSky();
    mTerrain = mNodeManager->GetTerrain();
    mHaze = mNodeManager->GetHaze();
}

void Canavar::Editor::ImGuiWidget::DrawWidget()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug");
    DrawNodes();

    ImGui::BeginDisabled(mSelectedNode == nullptr);
    DrawNode(mSelectedNode);
    ImGui::EndDisabled();

    DrawSky();
    DrawSun();
    DrawHaze();
    DrawTerrain();
    DrawRenderSettings();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void Canavar::Editor::ImGuiWidget::DrawRenderSettings()
{
    if (ImGui::CollapsingHeader("Render Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderInt("Blur Pass", &mRenderingManager->GetBlurPass_NonConst(), 0, 24);
    }
}

void Canavar::Editor::ImGuiWidget::DrawNodes()
{
    const auto &nodes = mNodeManager->GetNodes();

    if (ImGui::BeginCombo("Select a node", mSelectedNode ? mSelectedNode->GetNodeName().toStdString().c_str() : "-"))
    {
        for (const auto pNode : nodes)
        {
            if (ImGui::Selectable(pNode->GetNodeName().toStdString().c_str()))
            {
                mSelectedNode = pNode;
            }
        }

        ImGui::EndCombo();
    }
}

void Canavar::Editor::ImGuiWidget::DrawNode(Engine::NodePtr pNode)
{
    if (ImGui::CollapsingHeader("Selected Node##DrawNode", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (pNode)
        {
            ImGui::Text("Node Parameters");

            auto WorldPosition = pNode->GetWorldPosition();
            if (ImGui::InputFloat3("World Position##Node", &WorldPosition[0], "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                pNode->SetWorldPosition(WorldPosition);
            }

            auto LocalPosition = pNode->GetPosition();
            if (ImGui::InputFloat3("Position##Node", &LocalPosition[0], "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                pNode->SetPosition(LocalPosition);
            }

            if (ImGui::SliderFloat("Yaw##NodeRotation", &pNode->GetYaw(), 0.0f, 360.0f, "%.3f"))
                pNode->UpdateRotationFromEulerDegrees();
            if (ImGui::SliderFloat("Pitch##NodeRotation", &pNode->GetPitch(), -89.999f, 89.999f, "%.3f"))
                pNode->UpdateRotationFromEulerDegrees();
            if (ImGui::SliderFloat("Roll##NodeRotation", &pNode->GetRoll(), -179.999f, 179.999f, "%.3f"))
                pNode->UpdateRotationFromEulerDegrees();

            auto Scale = pNode->GetScale();
            if (ImGui::InputFloat3("Scale##NodeScale", &Scale[0], "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                pNode->SetScale(Scale);
            }

            auto ScaleMin = std::min(Scale.x(), std::min(Scale.y(), Scale.z()));
            if (ImGui::DragFloat("Scale##NodeScaleDrag", &ScaleMin, 0.01f, 0.001, 100.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                pNode->SetScale(ScaleMin, ScaleMin, ScaleMin);
            }

            if (ImGui::Button("Go to node"))
            {
                emit GoToNode(pNode);
            }
        }

        if (Engine::ModelPtr pModel = std::dynamic_pointer_cast<Engine::Model>(pNode))
        {
            ImGui::Text("Model Parameters");

            ImGui::SliderFloat("Ambient##Model", &pModel->GetAmbient_NonConst(), 0.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("Diffuse##Model", &pModel->GetDiffuse_NonConst(), 0.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("Specular##Model", &pModel->GetSpecular_NonConst(), 0.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("Shininess##Model", &pModel->GetShininess_NonConst(), 1.0f, 128.0f, "%.3f");
            ImGui::ColorEdit4("Color##PointLight", &pModel->GetColor_NonConst()[0]);
        }
        else if (Engine::DirectionalLightPtr pDirectionalLight = std::dynamic_pointer_cast<Engine::DirectionalLight>(pNode))
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

        else if (Engine::PointLightPtr pPointLight = std::dynamic_pointer_cast<Engine::PointLight>(pNode))
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
        else if (Engine::NozzleEffectPtr pNozzleEffect = std::dynamic_pointer_cast<Engine::NozzleEffect>(pNode))
        {
            ImGui::Text("Nozzle Effect Parameters");

            ImGui::SliderFloat("Max Radius##NozzleEffect", &pNozzleEffect->GetMaxRadius_NonConst(), 0.001f, 4.0f, "%.4f");
            ImGui::SliderFloat("Max Life##NozzleEffect", &pNozzleEffect->GetMaxLife_NonConst(), 0.0000f, 0.1f, "%.5f");
            ImGui::SliderFloat("Max Distance##NozzleEffect", &pNozzleEffect->GetMaxDistance_NonConst(), 1.0f, 30.0f, "%.3f");
            ImGui::SliderFloat("Min Distance##NozzleEffect", &pNozzleEffect->GetMinDistance_NonConst(), 1.0f, 30.0f, "%.3f");
            ImGui::SliderFloat("Speed##NozzleEffect", &pNozzleEffect->GetSpeed_NonConst(), 0.0f, 10.0f, "%.5f");
            ImGui::SliderFloat("Scale##NozzleEffect", &pNozzleEffect->GetScale_NonConst(), 0.001f, 0.1f, "%.4f");
        }
    }
}

void Canavar::Editor::ImGuiWidget::DrawSky()
{
    if (ImGui::CollapsingHeader("Sky##DrawSky"))
    {
        ImGui::SliderFloat("Albedo##Sky", &mSky->GetAlbedo_NonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Turbidity##Sky", &mSky->GetTurbidity_NonConst(), 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("Normalized Sun Y##Sun", &mSky->GetNormalizedSunY_NonConst(), 0.0f, 10.0f, "%.3f");
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
