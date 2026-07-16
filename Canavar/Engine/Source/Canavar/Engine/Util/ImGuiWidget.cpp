#include "ImGuiWidget.h"

#include "Canavar/Engine/Camera/Camera.h"
#include "Canavar/Engine/Camera/FreeCamera.h"
#include "Canavar/Engine/Camera/PersecutorCamera.h"
#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/GlobalNode/Haze/Haze.h"
#include "Canavar/Engine/GlobalNode/Sky/Sky.h"
#include "Canavar/Engine/GlobalNode/Terrain/Terrain.h"
#include "Canavar/Engine/Light/DirectionalLight.h"
#include "Canavar/Engine/Light/Light.h"
#include "Canavar/Engine/Light/PointLight.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Manager/TexturedModelRenderer.h"
#include "Canavar/Engine/Model/PbrMaterial.h"
#include "Canavar/Engine/Model/PhongMaterial.h"
#include "Canavar/Engine/Model/TexturedModel/TexturedModel.h"
#include "Canavar/Engine/Node/Node.h"
#include "Canavar/Engine/Object/Object.h"

#include <algorithm>
#include <cstring>
#include <imgui.h>

#include <QQuaternion>
#include <QVector3D>
#include <QtImGui.h>

// ─────────────────────────────────────────────────────────────────────────────
// Construction / lifecycle
// ─────────────────────────────────────────────────────────────────────────────

Canavar::Engine::ImGuiWidget::ImGuiWidget(Renderer *pRenderer)
    : mRenderer(pRenderer)
{
    connect(mRenderer, &Renderer::Initialized, this, &Canavar::Engine::ImGuiWidget::Initialize);
    connect(mRenderer, &Renderer::CanRenderOverlay, this, &Canavar::Engine::ImGuiWidget::OnRenderOverlay);

    mNodeManager = mRenderer->GetNodeManager();
    mCameraManager = mRenderer->GetCameraManager();
}

void Canavar::Engine::ImGuiWidget::Initialize()
{
    mRenderRef = QtImGui::initialize(mRenderer->GetOpenGLWidget(), false);
}

void Canavar::Engine::ImGuiWidget::OnRenderOverlay(float Ifps)
{
    QtImGui::newFrame(mRenderRef);
    DrawImGuiWidgets(Ifps);
    ImGui::Render();
    QtImGui::render(mRenderRef);
}

// ─────────────────────────────────────────────────────────────────────────────
// Top-level
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawImGuiWidgets(float Ifps)
{
    // Invalidate selection if the node has been deleted.
    ValidateSelectedNode();

    // Main scene-editor window.
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Scene Editor##DrawImGuiWidgets", nullptr, ImGuiWindowFlags_MenuBar);
    DrawMenuBar();
    DrawNodeList();
    DrawSkyProperties();
    DrawHazeProperties();
    DrawTerrainProperties();
    DrawStats(Ifps);
    ImGui::End();

    // Properties window – only shown when a node is selected.
    if (mSelectedNode)
    {
        ImGui::SetNextWindowSize(ImVec2(380, 520), ImGuiCond_FirstUseEver);
        ImGui::Begin("Node Properties##DrawImGuiWidgets");
        DrawNodeProperties();
        ImGui::End();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Menu Bar
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawMenuBar()
{
    if (!ImGui::BeginMenuBar())
    {
        return;
    }

    if (ImGui::BeginMenu("Add"))
    {
        // ── Cameras ──────────────────────────────────────────────────────────
        if (ImGui::BeginMenu("Camera##DrawMenuBar"))
        {
            if (ImGui::MenuItem("Free Camera"))
            {
                mSelectedNode = mNodeManager->CreateCamera<FreeCamera>();
            }

            if (ImGui::MenuItem("Persecutor Camera"))
            {
                mSelectedNode = mNodeManager->CreateCamera<PersecutorCamera>();
            }

            ImGui::EndMenu();
        }

        // ── Lights ───────────────────────────────────────────────────────────
        if (ImGui::BeginMenu("Light##DrawMenuBar"))
        {
            if (ImGui::MenuItem("Point Light"))
            {
                mSelectedNode = mNodeManager->CreateLight<PointLight>();
            }

            if (ImGui::MenuItem("Directional Light"))
            {
                mSelectedNode = mNodeManager->CreateLight<DirectionalLight>();
            }

            ImGui::EndMenu();
        }

        // ── Models ───────────────────────────────────────────────────────────
        // Only scene names that are already loaded by TexturedModelRenderer are
        // offered, because the renderer aborts on unknown model names.
        if (ImGui::BeginMenu("Model##DrawMenuBar"))
        {
            const auto &Scenes = mRenderer->GetTexturedModelRenderer()->GetScenes();

            if (Scenes.isEmpty())
            {
                ImGui::TextDisabled("No models available.");
                ImGui::TextDisabled("Place .obj / .glb / .gltf / .ply in:");
                ImGui::TextDisabled("%s", MODELS_FOLDER);
            }
            else
            {
                for (auto Iterator = Scenes.constBegin(); Iterator != Scenes.constEnd(); ++Iterator)
                {
                    const auto &ModelName = Iterator.key();
                    if (ImGui::MenuItem(ModelName.toStdString().c_str()))
                    {
                        mSelectedNode = mNodeManager->CreateTexturedModel<TexturedModel>(ModelName);
                    }
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

// ─────────────────────────────────────────────────────────────────────────────
// Stats
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawStats(float)
{
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

// ─────────────────────────────────────────────────────────────────────────────
// Node List
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawNodeList()
{
    // ── Cameras ──────────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Cameras##DrawNodeList"))
    {
        for (const auto &pCamera : mNodeManager->GetCameras())
        {
            ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if (mSelectedNode == pCamera.get())
            {
                Flags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(pCamera->GetNodeUniqueName().c_str(), Flags);
            if (ImGui::IsItemClicked())
            {
                mSelectedNode = pCamera.get();
            }
        }
    }

    // ── Lights ───────────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Lights##DrawNodeList"))
    {
        for (const auto &pLight : mNodeManager->GetLights())
        {
            ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if (mSelectedNode == pLight.get())
            {
                Flags |= ImGuiTreeNodeFlags_Selected;
            }
            ImGui::TreeNodeEx(pLight->GetNodeUniqueName().c_str(), Flags);

            if (ImGui::IsItemClicked())
            {
                mSelectedNode = pLight.get();
            }
        }
    }

    // ── Models ───────────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Models##DrawNodeList"))
    {
        for (const auto &pModel : mNodeManager->GetTexturedModels())
        {
            ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if (mSelectedNode == pModel.get())
            {
                Flags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(pModel->GetNodeUniqueName().c_str(), Flags);
            if (ImGui::IsItemClicked())
            {
                mSelectedNode = pModel.get();
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Node Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawNodeProperties()
{
    // Refresh the name buffer whenever the selected node changes.
    if (mSelectedNode != mLastSelectedNode)
    {
        mLastSelectedNode = mSelectedNode;
        const auto &Name = mSelectedNode->GetNodeName();
        std::strncpy(mNodeNameBuffer, Name.c_str(), sizeof(mNodeNameBuffer) - 1);
        mNodeNameBuffer[sizeof(mNodeNameBuffer) - 1] = '\0';
    }

    // ── Header info ──────────────────────────────────────────────────────────
    if (ImGui::InputText("Name##DrawNodeProperties", mNodeNameBuffer, sizeof(mNodeNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        mSelectedNode->SetNodeName(std::string(mNodeNameBuffer));
    }

    ImGui::Text("Type : %s", mSelectedNode->GetNodeTypeName());
    ImGui::Text("ID   : %d", mSelectedNode->GetNodeId());

    // ── Dispatch to per-type drawers ─────────────────────────────────────────
    // Derived-most types are tested first so PointLight is not accidentally
    // caught by a Light* branch before reaching the PointLight* branch.
    if (auto *pPointLight = dynamic_cast<PointLight *>(mSelectedNode))
    {
        DrawObjectTransform(pPointLight);
        DrawLightProperties(pPointLight);
        DrawPointLightProperties(pPointLight);
    }
    else if (auto *pDirLight = dynamic_cast<DirectionalLight *>(mSelectedNode))
    {
        DrawObjectTransform(pDirLight);
        DrawLightProperties(pDirLight);
        DrawDirectionalLightProperties(pDirLight);
    }
    else if (auto *pCamera = dynamic_cast<Camera *>(mSelectedNode))
    {
        DrawObjectTransform(pCamera);
        DrawCameraProperties(pCamera);
    }
    else if (auto *pModel = dynamic_cast<TexturedModel *>(mSelectedNode))
    {
        DrawObjectTransform(pModel);
        DrawTexturedModelProperties(pModel);
    }

    // ── Delete button ─────────────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.10f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.20f, 0.20f, 1.0f));
    if (ImGui::Button("Delete Node##DrawNodeProperties", ImVec2(-1, 0)))
    {
        Node *pToDelete = mSelectedNode;
        mSelectedNode = nullptr;
        mLastSelectedNode = nullptr;
        mRenderer->GetNodeManager()->RemoveNode(pToDelete);
    }
    ImGui::PopStyleColor(2);
}

// ─────────────────────────────────────────────────────────────────────────────
// Transform
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawObjectTransform(Object *pObject)
{
    if (!ImGui::CollapsingHeader("Transform##DrawNodeProperties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    ImGui::Checkbox("Visible##DrawNodeProperties", &pObject->GetVisible_NonConst());

    // Position
    {
        const auto &Position = pObject->GetPosition();
        float Vector[3] = { Position.x(), Position.y(), Position.z() };
        if (ImGui::DragFloat3("Position##DrawNodeProperties", Vector, 0.1f))
        {
            pObject->SetPosition(Vector[0], Vector[1], Vector[2]);
        }
    }

    // Rotation – stored as a quaternion, edited here as Euler angles (pitch / yaw / roll).
    {
        float Pitch, Yaw, Roll;
        pObject->GetRotation().getEulerAngles(&Pitch, &Yaw, &Roll);
        float Vector[3] = { Pitch, Yaw, Roll };
        if (ImGui::DragFloat3("Rotation (P/Y/R)##DrawNodeProperties", Vector, 0.5f))
        {
            pObject->SetRotation(QQuaternion::fromEulerAngles(Vector[0], Vector[1], Vector[2]));
        }
    }

    // Scale
    {
        const auto &Scale = pObject->GetScale();
        float Vector[3] = { Scale.x(), Scale.y(), Scale.z() };
        if (ImGui::DragFloat3("Scale##DrawNodeProperties", Vector, 0.01f, 0.001f, 10000.0f))
        {
            pObject->SetScale(Vector[0], Vector[1], Vector[2]);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Camera Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawCameraProperties(Camera *pCamera)
{
    if (!ImGui::CollapsingHeader("Camera##DrawCameraProperties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    // All concrete cameras in this engine derive from PerspectiveCamera.
    auto *pPersp = dynamic_cast<PerspectiveCamera *>(pCamera);

    if (!pPersp)
    {
        return;
    }

    const bool IsActive = (mCameraManager->GetActiveCamera() == pPersp);

    if (IsActive)
    {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Active Camera");
    }
    else if (ImGui::Button("Set as Active Camera##DrawCameraProperties"))
    {
        mCameraManager->SetActiveCamera(pPersp);
    }

    ImGui::DragFloat("Z Near##DrawCameraProperties", &pPersp->GetZNear_NonConst(), 0.01f, 0.001f, 1000.0f);

    ImGui::DragFloat("Z Far##DrawCameraProperties", &pPersp->GetZFar_NonConst(), 100.0f, 1.0f, 10'000'000.0f);

    ImGui::SliderFloat("Vertical FOV##DrawCameraProperties", &pPersp->GetVerticalFov_NonConst(), 1.0f, 170.0f);

    // ── PersecutorCamera extras ───────────────────────────────────────────────
    if (auto *pPersecutor = dynamic_cast<PersecutorCamera *>(pCamera))
    {
        ImGui::TextUnformatted("Persecutor Camera");
        ImGui::DragFloat("Distance##DrawCameraProperties", &pPersecutor->GetDistance_NonConst(), 0.1f, 0.1f, 20000.0f);
        ImGui::DragFloat("Angular Speed##DrawCameraProperties", &pPersecutor->GetAngularSpeed_NonConst(), 0.5f, 0.0f, 720.0f);
        ImGui::DragFloat("Linear Speed##DrawCameraProperties", &pPersecutor->GetLinearSpeed_NonConst(), 0.01f, 0.0f, 1000.0f);
        ImGui::DragFloat("Zoom Step##DrawCameraProperties", &pPersecutor->GetZoomStep_NonConst(), 0.01f, 0.01f, 100.0f);
    }
    // ── FreeCamera extras ─────────────────────────────────────────────────────
    else if (dynamic_cast<FreeCamera *>(pCamera))
    {
        ImGui::TextUnformatted("Free Camera");
        ImGui::TextDisabled("Controlled via keyboard + mouse");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Light Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawLightProperties(Light *pLight)
{
    if (!ImGui::CollapsingHeader("Light##DrawLightProperties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    ImGui::Checkbox("Enabled##DrawLightProperties", &pLight->GetEnabled_NonConst());

    const auto &Color = pLight->GetColor();
    float Vector[3] = { Color.x(), Color.y(), Color.z() };
    if (ImGui::ColorEdit3("Color##DrawLightProperties", Vector))
    {
        pLight->SetColor(QVector3D(Vector[0], Vector[1], Vector[2]));
    }

    ImGui::DragFloat("Ambient##DrawLightProperties", &pLight->GetAmbient_NonConst(), 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Diffuse##DrawLightProperties", &pLight->GetDiffuse_NonConst(), 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Specular##DrawLightProperties", &pLight->GetSpecular_NonConst(), 0.01f, 0.0f, 10.0f);
}

void Canavar::Engine::ImGuiWidget::DrawPointLightProperties(PointLight *pLight)
{
    if (!ImGui::CollapsingHeader("Attenuation##DrawPointLightProperties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    ImGui::DragFloat("Constant##DrawPointLightProperties", &pLight->GetConstant_NonConst(), 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Linear##DrawPointLightProperties", &pLight->GetLinear_NonConst(), 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat("Quadratic##DrawPointLightProperties", &pLight->GetQuadratic_NonConst(), 0.0001f, 0.0f, 1.0f, "%.4f");
}

void Canavar::Engine::ImGuiWidget::DrawDirectionalLightProperties(DirectionalLight *pLight)
{
    if (!ImGui::CollapsingHeader("Directional Light##DrawDirectionalLightProperties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    {
        const auto &Direction = pLight->GetDirection();
        float Vector[3] = { Direction.x(), Direction.y(), Direction.z() };
        if (ImGui::DragFloat3("Direction##DrawDirectionalLightProperties", Vector, 0.01f, -1.0f, 1.0f))
        {
            pLight->SetDirection(QVector3D(Vector[0], Vector[1], Vector[2]));
        }
    }

    ImGui::DragFloat("Radiance##DrawDirectionalLightProperties", &pLight->GetRadiance_NonConst(), 0.1f, 0.0f, 100.0f);
}

// ─────────────────────────────────────────────────────────────────────────────
// TexturedModel Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawTexturedModelProperties(TexturedModel *pModel)
{
    if (!ImGui::CollapsingHeader("Textured Model##DrawTexturedModelProperties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    ImGui::LabelText("Model", "%s", pModel->GetModelUniqueNameStdString().c_str());

    // Shading mode
    {
        int Mode = static_cast<int>(pModel->GetShadingMode());
        const char *Modes[] = { "PBR", "Phong" };
        if (ImGui::Combo("Shading Mode", &Mode, Modes, IM_ARRAYSIZE(Modes)))
        {
            pModel->SetShadingMode(static_cast<ShadingMode>(Mode));
        }
    }

    ImGui::DragFloat("Opacity##DrawTexturedModelProperties", &pModel->GetOpacity_NonConst(), 0.01f, 0.0f, 1.0f);

    ImGui::Checkbox("Use Color Override##DrawTexturedModelProperties", &pModel->GetUseColor_NonConst());

    if (pModel->GetUseColor())
    {
        const auto &Color = pModel->GetColor();
        float Vector[3] = { Color.x(), Color.y(), Color.z() };
        if (ImGui::ColorEdit3("Color Override##DrawTexturedModelProperties", Vector))
        {
            pModel->SetColor(QVector3D(Vector[0], Vector[1], Vector[2]));
        }
    }

    ImGui::Checkbox("Overlay Pass##DrawTexturedModelProperties", &pModel->GetOverlay_NonConst());

    // ── Material ──────────────────────────────────────────────────────────────
    if (pModel->GetShadingMode() == ShadingMode::Pbr)
    {
        if (ImGui::CollapsingHeader("PBR Material##DrawTexturedModelProperties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            PbrMaterial &Material = pModel->GetPbrMaterial_NonConst();
            ImGui::SliderFloat("Metallic##DrawTexturedModelProperties", &Material.Metallic, 0.0f, 1.0f);
            ImGui::SliderFloat("Roughness##DrawTexturedModelProperties", &Material.Roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("Ambient Occlusion##DrawTexturedModelProperties", &Material.AmbientOcclusion, 0.0f, 1.0f);
        }
    }
    else
    {
        if (ImGui::CollapsingHeader("Phong Material##DrawTexturedModelProperties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            PhongMaterial &Material = pModel->GetPhongMaterial_NonConst();
            ImGui::SliderFloat("Ambient##DrawTexturedModelProperties", &Material.Ambient, 0.0f, 1.0f);
            ImGui::SliderFloat("Diffuse##DrawTexturedModelProperties", &Material.Diffuse, 0.0f, 1.0f);
            ImGui::SliderFloat("Specular##DrawTexturedModelProperties", &Material.Specular, 0.0f, 1.0f);
            ImGui::SliderFloat("Shininess##DrawTexturedModelProperties", &Material.Shininess, 1.0f, 512.0f);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Sky Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawSkyProperties()
{
    Sky *pSky = mRenderer->GetSky();
    if (!pSky)
    {
        return;
    }

    if (ImGui::CollapsingHeader("Sky##DrawSkyProperties"))
    {
        ImGui::Checkbox("Enabled##DrawSkyProperties", &pSky->GetEnabled_NonConst());

        ImGui::DragFloat("Sun Intensity##DrawSkyProperties", &pSky->GetSunIntensity_NonConst(), 0.1f, 0.0f, 200.0f);
        ImGui::DragFloat("Planet Radius##DrawSkyProperties", &pSky->GetPlanetRadius_NonConst(), 1000.0f, 1.0e3f, 1.0e7f, "%.0f m");
        ImGui::DragFloat("Atmosphere Radius##DrawSkyProperties", &pSky->GetAtmosphereRadius_NonConst(), 1000.0f, 1.0e3f, 1.0e7f, "%.0f m");
        ImGui::DragFloat("Scale Height R##DrawSkyProperties", &pSky->GetScaleHeightR_NonConst(), 10.0f, 0.0f, 50000.0f, "%.0f m");
        ImGui::DragFloat("Scale Height M##DrawSkyProperties", &pSky->GetScaleHeightM_NonConst(), 10.0f, 0.0f, 50000.0f, "%.0f m");
        ImGui::DragFloat("Beta Mie##DrawSkyProperties", &pSky->GetBetaMie_NonConst(), 1e-7f, 0.0f, 1e-3f, "%.2e");
        ImGui::DragFloat("Mie G##DrawSkyProperties", &pSky->GetMieG_NonConst(), 0.001f, 0.0f, 0.9999f);
        ImGui::DragFloat("Horizon Offset##DrawSkyProperties", &pSky->GetHorizonOffset_NonConst(), 0.001f, -1.0f, 1.0f);

        {
            const auto &Beta = pSky->GetBetaRayleigh();
            float Vector[3] = { Beta.x(), Beta.y(), Beta.z() };
            if (ImGui::DragFloat3("Beta Rayleigh##DrawSkyProperties", Vector, 1e-7f, 0.0f, 1e-3f, "%.2e"))
            {
                pSky->SetBetaRayleigh(QVector3D(Vector[0], Vector[1], Vector[2]));
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Haze Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawHazeProperties()
{
    Haze *pHaze = mRenderer->GetHaze();
    if (!pHaze)
    {
        return;
    }

    if (ImGui::CollapsingHeader("Haze##DrawHazeProperties"))
    {
        ImGui::Checkbox("Enabled##DrawHazeProperties", &pHaze->GetEnabled_NonConst());

        {
            const auto &Color = pHaze->GetColor();
            float Vector[3] = { Color.x(), Color.y(), Color.z() };
            if (ImGui::ColorEdit3("Color##DrawHazeProperties", Vector))
            {
                pHaze->SetColor(QVector3D(Vector[0], Vector[1], Vector[2]));
            }
        }

        ImGui::DragFloat("Density##DrawHazeProperties", &pHaze->GetDensity_NonConst(), 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Gradient##DrawHazeProperties", &pHaze->GetGradient_NonConst(), 0.01f, 0.0f, 10.0f);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Terrain Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawTerrainProperties()
{
    Terrain *pTerrain = mRenderer->GetTerrain();
    if (!pTerrain)
    {
        return;
    }

    if (ImGui::CollapsingHeader("Terrain##DrawTerrainProperties"))
    {
        ImGui::Checkbox("Enabled##DrawTerrainProperties", &pTerrain->GetEnabled_NonConst());

        // ── Noise ────────────────────────────────────────────────────────────────────
        ImGui::Text("Noise");
        ImGui::SliderInt("Octaves##DrawTerrainProperties", &pTerrain->GetOctaves_NonConst(), 1, 16);
        ImGui::DragFloat("Amplitude##DrawTerrainProperties", &pTerrain->GetAmplitude_NonConst(), 1.0f, 0.0f, 10000.0f);
        ImGui::DragFloat("Frequency##DrawTerrainProperties", &pTerrain->GetFrequency_NonConst(), 0.001f, 0.0f, 1.0f, "%.4f");
        ImGui::DragFloat("Persistence##DrawTerrainProperties", &pTerrain->GetPersistence_NonConst(), 0.001f, 0.0f, 1.0f, "%.4f");
        ImGui::DragFloat("Lacunarity##DrawTerrainProperties", &pTerrain->GetLacunarity_NonConst(), 0.01f, 0.0f, 32.0f);

        // ── Tessellation ──────────────────────────────────────────────────────────
        ImGui::Text("Tessellation");
        ImGui::SliderFloat("Multiplier##DrawTerrainProperties", &pTerrain->GetTessellationMultiplier_NonConst(), 1.0f, 64.0f);

        // ── Lighting ─────────────────────────────────────────────────────────────
        ImGui::Text("Lighting");
        ImGui::SliderFloat("Ambient##DrawTerrainProperties", &pTerrain->GetAmbient_NonConst(), 0.0f, 1.0f);
        ImGui::SliderFloat("Diffuse##DrawTerrainProperties", &pTerrain->GetDiffuse_NonConst(), 0.0f, 1.0f);
        ImGui::SliderFloat("Specular##DrawTerrainProperties", &pTerrain->GetSpecular_NonConst(), 0.0f, 1.0f);
        ImGui::SliderFloat("Shininess##DrawTerrainProperties", &pTerrain->GetShininess_NonConst(), 1.0f, 512.0f);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::ValidateSelectedNode()
{
    if (!mSelectedNode)
    {
        return;
    }

    const auto &Nodes = mRenderer->GetNodeManager()->GetNodes();
    const bool StillAlive = std::any_of(Nodes.begin(), Nodes.end(), [this](Node *pNode) { return pNode == mSelectedNode; });
    if (!StillAlive)
    {
        mSelectedNode = nullptr;
        mLastSelectedNode = nullptr;
    }
}
