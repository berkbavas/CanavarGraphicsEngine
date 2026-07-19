#include "ImGuiWidget.h"

#include "Canavar/Engine/Camera/Camera.h"
#include "Canavar/Engine/Camera/FreeCamera.h"
#include "Canavar/Engine/Camera/PersecutorCamera.h"
#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/Enums.h"
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
#include "Canavar/Engine/Model/PrimitiveModel/PrimitiveModel.h"
#include "Canavar/Engine/Model/PrimitiveModel/Primitives.h"
#include "Canavar/Engine/Model/TexturedModel/TexturedModel.h"
#include "Canavar/Engine/Node/Node.h"
#include "Canavar/Engine/Object/Object.h"
#include "Canavar/Engine/Util/Gizmo.h"

#include <algorithm>
#include <cstring>
#include <imgui.h>

#include <QFileDialog>
#include <QQuaternion>
#include <QVector3D>
#include <QtImGui.h>

// ─────────────────────────────────────────────────────────────────────────────
// Construction / lifecycle
// ─────────────────────────────────────────────────────────────────────────────

Canavar::Engine::ImGuiWidget::ImGuiWidget(Renderer *pRenderer)
    : mRenderer(pRenderer)
{
    // Connect to the renderer's signals for initialization and overlay rendering
    connect(mRenderer, &Renderer::Initialized, this, &Canavar::Engine::ImGuiWidget::Initialize);
    connect(mRenderer, &Renderer::PostRender, this, &Canavar::Engine::ImGuiWidget::OnPostRender);

    // Retrieve the NodeManager and CameraManager from the renderer for later use
    mNodeManager = mRenderer->GetNodeManager();
    mCameraManager = mRenderer->GetCameraManager();

    // Register this ImGuiWidget as an event receiver to handle input events
    mRenderer->AddEventReceiver(this);
    mRenderer->AddEventThief(this); // Register as an event thief to capture input events before they reach other receivers
}

bool Canavar::Engine::ImGuiWidget::WantsKeyboardCapture() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool Canavar::Engine::ImGuiWidget::WantsMouseCapture() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

void Canavar::Engine::ImGuiWidget::Initialize()
{
    mRenderRef = QtImGui::initialize(mRenderer->GetOpenGLWidget(), false);
}

void Canavar::Engine::ImGuiWidget::OnPostRender(float Ifps)
{
    QtImGui::newFrame(mRenderRef);
    DrawImGuiWidgets(Ifps);
    emit CanDrawImGuiWidgets(Ifps);
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
    DrawPostProcessPanel();
    DrawRendererProperties();
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
                const auto pNode = mNodeManager->CreateNode<FreeCamera>();
                SetSelectedNode(pNode);
            }

            if (ImGui::MenuItem("Persecutor Camera"))
            {
                const auto pNode = mNodeManager->CreateNode<PersecutorCamera>();
                SetSelectedNode(pNode);
            }

            ImGui::EndMenu();
        }

        // ── Lights ───────────────────────────────────────────────────────────
        if (ImGui::BeginMenu("Light##DrawMenuBar"))
        {
            if (ImGui::MenuItem("Point Light"))
            {
                const auto pNode = mNodeManager->CreateNode<PointLight>();
                SetSelectedNode(pNode);
            }

            if (ImGui::MenuItem("Directional Light"))
            {
                const auto pNode = mNodeManager->CreateNode<DirectionalLight>();
                SetSelectedNode(pNode);
            }

            ImGui::EndMenu();
        }

        // ── Models ───────────────────────────────────────────────────────────
        // Only scene names that are already loaded by TexturedModelRenderer are
        // offered, because the renderer aborts on unknown model names.
        if (ImGui::BeginMenu("Model##DrawMenuBar"))
        {
            const auto &Scenes = mRenderer->GetScenes();

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
                        const auto pNode = mNodeManager->CreateNode<TexturedModel>(ModelName);
                        SetSelectedNode(pNode);
                    }
                }
            }

            ImGui::EndMenu();
        }

        // ── Primitives ────────────────────────────────────────────────────────
        if (ImGui::BeginMenu("Primitive##DrawMenuBar"))
        {
            if (ImGui::MenuItem("Circle"))
            {
                const auto pNode = mNodeManager->CreateNode<Circle>();
                SetSelectedNode(pNode);
            }
            if (ImGui::MenuItem("Disk"))
            {
                const auto pNode = mNodeManager->CreateNode<Disk>();
                SetSelectedNode(pNode);
            }
            if (ImGui::MenuItem("Line"))
            {
                const auto pNode = mNodeManager->CreateNode<Line>();
                SetSelectedNode(pNode);
            }
            if (ImGui::MenuItem("Plane"))
            {
                const auto pNode = mNodeManager->CreateNode<Plane>();
                SetSelectedNode(pNode);
            }
            if (ImGui::MenuItem("Sphere"))
            {
                const auto pNode = mNodeManager->CreateNode<Sphere>();
                SetSelectedNode(pNode);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }

    // ── Scene ─────────────────────────────────────────────────────────────────
    if (ImGui::BeginMenu("Scene"))
    {
        if (ImGui::MenuItem("Export Nodes..."))
        {
            const QString FilePath = QFileDialog::getSaveFileName(mRenderer->GetOpenGLWidget(), "Export Nodes", QString(), "JSON files (*.json);;All files (*)");

            if (!FilePath.isEmpty())
                mNodeManager->ExportNodes(FilePath.toStdString());
        }

        if (ImGui::MenuItem("Import Nodes..."))
        {
            const QString FilePath = QFileDialog::getOpenFileName(mRenderer->GetOpenGLWidget(), "Import Nodes", QString(), "JSON files (*.json);;All files (*)");

            if (!FilePath.isEmpty())
                mNodeManager->ImportNodes(FilePath.toStdString());
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

void Canavar::Engine::ImGuiWidget::DrawNodeTree(Node *pNode)
{
    const bool HasChildren = !pNode->GetChildren().empty();

    ImGuiTreeNodeFlags Flags = HasChildren ? //
                                   (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)
                                           : (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

    if (mSelectedNode == pNode)
    {
        Flags |= ImGuiTreeNodeFlags_Selected;
    }

    const auto *pNodeId = reinterpret_cast<void *>(static_cast<intptr_t>(pNode->GetNodeId()));
    const bool Open = ImGui::TreeNodeEx(pNodeId, Flags, "[%s]  %s", pNode->GetNodeTypeName(), pNode->GetNodeUniqueName().c_str());

    if (ImGui::IsItemClicked())
    {
        SetSelectedNode(pNode);
    }

    if (HasChildren && Open)
    {
        for (Node *pChild : pNode->GetChildren())
        {
            DrawNodeTree(pChild);
        }
        ImGui::TreePop();
    }
}

void Canavar::Engine::ImGuiWidget::DrawNodeList()
{
    if (!ImGui::CollapsingHeader("Scene Hierarchy##DrawNodeList", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    // Only draw root nodes; children are drawn recursively via DrawNodeTree.
    const auto DrawRootsFrom = [this](auto &List) {
        for (const auto &pOwned : List)
        {
            if (!pOwned->GetParent())
            {
                DrawNodeTree(pOwned);
            }
        }
    };

    DrawRootsFrom(mNodeManager->GetObjects());
}

// ─────────────────────────────────────────────────────────────────────────────
// Node Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawNodeProperties()
{
    // ── Header info ──────────────────────────────────────────────────────────
    if (ImGui::InputText("Name##DrawNodeProperties", mNodeNameBuffer, sizeof(mNodeNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        mSelectedNode->SetNodeName(std::string(mNodeNameBuffer));
    }

    ImGui::Text("Type : %s", mSelectedNode->GetNodeTypeName());
    ImGui::Text("ID   : %d", mSelectedNode->GetNodeId());

    // ── Hierarchy ─────────────────────────────────────────────────────────────
    DrawHierarchyProperties(mSelectedNode);

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
    else if (auto *pPrimitive = dynamic_cast<PrimitiveModel *>(mSelectedNode))
    {
        DrawObjectTransform(pPrimitive);
        DrawPrimitiveModelProperties(pPrimitive);
    }

    // ── Delete button ─────────────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.10f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.20f, 0.20f, 1.0f));
    if (ImGui::Button("Delete Node##DrawNodeProperties", ImVec2(-1, 0)))
    {
        Node *pToDelete = mSelectedNode;
        SetSelectedNode(nullptr);
        mRenderer->GetNodeManager()->RemoveNode(pToDelete);
    }
    ImGui::PopStyleColor(2);
}

// ─────────────────────────────────────────────────────────────────────────────
// Hierarchy Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawHierarchyProperties(Node *pNode)
{
    if (!ImGui::CollapsingHeader("Hierarchy##DrawHierarchyProperties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    // ── Current parent ────────────────────────────────────────────────────────
    Node *pParent = pNode->GetParent();
    if (pParent)
    {
        ImGui::LabelText("Parent##DrawHierarchyProperties", "%s", pParent->GetNodeUniqueName().c_str());

        if (ImGui::Button("Detach from Parent##DrawHierarchyProperties"))
        {
            pNode->SetParent(nullptr);
        }
    }
    else
    {
        ImGui::TextDisabled("No parent (root node)");
    }

    // ── Children ─────────────────────────────────────────────────────────────
    const auto &Children = pNode->GetChildren();
    if (!Children.empty())
    {
        ImGui::Separator();
        ImGui::Text("Children (%d):", static_cast<int>(Children.size()));
        for (Node *pChild : Children)
        {
            ImGui::BulletText("%s", pChild->GetNodeUniqueName().c_str());
        }
    }

    // ── Attach to a new parent ────────────────────────────────────────────────
    ImGui::Separator();
    if (ImGui::BeginCombo("Set Parent##DrawHierarchyProperties", "Select node..."))
    {
        const auto TryListNodes = [&](auto &List) {
            for (const auto &pOwned : List)
            {
                Node *pCandidate = pOwned;

                // Skip self, current parent, and existing children to avoid trivial cycles.
                if (pCandidate == pNode || pCandidate == pParent)
                {
                    continue;
                }

                // Guard: skip if pNode is already an ancestor of pCandidate.
                bool IsCycle = false;
                Node *pAncestor = pCandidate->GetParent();
                while (pAncestor)
                {
                    if (pAncestor == pNode)
                    {
                        IsCycle = true;
                        break;
                    }
                    pAncestor = pAncestor->GetParent();
                }
                if (IsCycle)
                {
                    continue;
                }

                if (ImGui::Selectable(pCandidate->GetNodeUniqueName().c_str()))
                {
                    pNode->SetParent(pCandidate);
                }
            }
        };

        TryListNodes(mNodeManager->GetCameras());
        TryListNodes(mNodeManager->GetLights());
        TryListNodes(mNodeManager->GetTexturedModels());
        TryListNodes(mNodeManager->GetPrimitiveModels());

        ImGui::EndCombo();
    }
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

    // ── Local transform ───────────────────────────────────────────────────────
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

    // ── World-space readout (only when a parent is present) ───────────────────
    if (pObject->GetParent())
    {
        ImGui::Separator();
        ImGui::TextDisabled("World Space (read-only)");

        {
            const auto WPos = pObject->GetWorldPosition();
            float Vector[3] = { WPos.x(), WPos.y(), WPos.z() };
            ImGui::InputFloat3("World Position##DrawNodeProperties", Vector, "%.3f", ImGuiInputTextFlags_ReadOnly);
        }

        {
            float Pitch, Yaw, Roll;
            pObject->GetWorldRotation().getEulerAngles(&Pitch, &Yaw, &Roll);
            float Vector[3] = { Pitch, Yaw, Roll };
            ImGui::InputFloat3("World Rotation##DrawNodeProperties", Vector, "%.3f", ImGuiInputTextFlags_ReadOnly);
        }

        {
            const auto WScale = pObject->GetWorldScale();
            float Vector[3] = { WScale.x(), WScale.y(), WScale.z() };
            ImGui::InputFloat3("World Scale##DrawNodeProperties", Vector, "%.3f", ImGuiInputTextFlags_ReadOnly);
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
        ImGui::DragFloat("Distance##DrawCameraProperties", &pPersecutor->GetDistance_NonConst(), 0.1f, 0.1f, 200.0f);
        ImGui::DragFloat("Angular Speed##DrawCameraProperties", &pPersecutor->GetAngularSpeed_NonConst(), 0.5f, 0.0f, 120.0f);
        ImGui::DragFloat("Linear Speed##DrawCameraProperties", &pPersecutor->GetLinearSpeed_NonConst(), 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Zoom Step##DrawCameraProperties", &pPersecutor->GetZoomStep_NonConst(), 0.01f, 0.01f, 5.0f);
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
// PrimitiveModel Properties
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawPrimitiveModelProperties(PrimitiveModel *pModel)
{
    if (!ImGui::CollapsingHeader("Primitive Model##DrawPrimitiveModelProperties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    // Color
    {
        const auto &Color = pModel->GetColor();
        float Vector[3] = { Color.x(), Color.y(), Color.z() };
        if (ImGui::ColorEdit3("Color##DrawPrimitiveModelProperties", Vector))
        {
            pModel->SetColor(QVector3D(Vector[0], Vector[1], Vector[2]));
        }
    }

    // Opacity
    ImGui::SliderFloat("Opacity##DrawPrimitiveModelProperties", &pModel->GetOpacity_NonConst(), 0.0f, 1.0f);
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
// Post-Process Effects Panel
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::ImGuiWidget::DrawPostProcessPanel()
{
    if (!ImGui::CollapsingHeader("Post-Process Effects##DrawPostProcessPanel"))
    {
        return;
    }

    // ── ACES Tone Mapping ─────────────────────────────────────────────────────
    {
        bool Enabled = mRenderer->GetPostProcessEffectEnabled(PostProcessEffectType::Aces);
        if (ImGui::Checkbox("ACES Tone Mapping##DrawPostProcessPanel", &Enabled))
        {
            mRenderer->SetPostProcessEffectEnabled(PostProcessEffectType::Aces, Enabled);
        }

        if (Enabled)
        {
            ImGui::Indent();
            AcesEffect *pAces = mRenderer->GetAcesEffect();
            ImGui::SliderFloat("Exposure##DrawPostProcessPanel_Aces", &pAces->GetExposure_NonConst(), 0.1f, 2.0f);
            ImGui::Unindent();
        }
    }

    ImGui::Separator();

    // ── Depth of Field ────────────────────────────────────────────────────────
    {
        bool Enabled = mRenderer->GetPostProcessEffectEnabled(PostProcessEffectType::DepthOfField);
        if (ImGui::Checkbox("Depth of Field##DrawPostProcessPanel", &Enabled))
        {
            mRenderer->SetPostProcessEffectEnabled(PostProcessEffectType::DepthOfField, Enabled);
        }

        if (Enabled)
        {
            ImGui::Indent();
            DepthOfFieldEffect *pDOF = mRenderer->GetDepthOfFieldEffect();
            ImGui::DragFloat("Focus Distance##DrawPostProcessPanel_DOF", &pDOF->GetFocusDistance_NonConst(), 1.0f, 0.1f, 1'000'000.0f);
            ImGui::DragFloat("Focus Range##DrawPostProcessPanel_DOF", &pDOF->GetFocusRange_NonConst(), 1.0f, 0.1f, 100'000.0f);
            ImGui::SliderFloat("Max Blur Radius (px)##DrawPostProcessPanel_DOF", &pDOF->GetMaxBlurRadius_NonConst(), 1.0f, 64.0f);
            ImGui::Unindent();
        }
    }

    ImGui::Separator();

    // ── Color Grading ─────────────────────────────────────────────────────────
    {
        bool Enabled = mRenderer->GetPostProcessEffectEnabled(PostProcessEffectType::ColorGrading);
        if (ImGui::Checkbox("Color Grading##DrawPostProcessPanel", &Enabled))
        {
            mRenderer->SetPostProcessEffectEnabled(PostProcessEffectType::ColorGrading, Enabled);
        }

        if (Enabled)
        {
            ImGui::Indent();
            ColorGradingEffect *pCG = mRenderer->GetColorGradingEffect();
            ImGui::SliderFloat("Brightness##DrawPostProcessPanel_CG", &pCG->GetBrightness_NonConst(), -1.0f, 1.0f);
            ImGui::SliderFloat("Contrast##DrawPostProcessPanel_CG", &pCG->GetContrast_NonConst(), 0.0f, 3.0f);
            ImGui::SliderFloat("Saturation##DrawPostProcessPanel_CG", &pCG->GetSaturation_NonConst(), 0.0f, 3.0f);
            ImGui::SliderFloat("Temperature##DrawPostProcessPanel_CG", &pCG->GetTemperature_NonConst(), -1.0f, 1.0f);
            ImGui::Unindent();
        }
    }

    ImGui::Separator();

    // ── Sharpen ───────────────────────────────────────────────────────────────
    {
        bool Enabled = mRenderer->GetPostProcessEffectEnabled(PostProcessEffectType::Sharpen);
        if (ImGui::Checkbox("Sharpen##DrawPostProcessPanel", &Enabled))
        {
            mRenderer->SetPostProcessEffectEnabled(PostProcessEffectType::Sharpen, Enabled);
        }

        if (Enabled)
        {
            ImGui::Indent();
            SharpenEffect *pSharpen = mRenderer->GetSharpenEffect();
            ImGui::SliderFloat("Strength##DrawPostProcessPanel_Sharpen", &pSharpen->GetStrength_NonConst(), 0.0f, 3.0f);
            ImGui::Unindent();
        }
    }

    ImGui::Separator();

    // ── FXAA ──────────────────────────────────────────────────────────────────
    {
        bool Enabled = mRenderer->GetPostProcessEffectEnabled(PostProcessEffectType::Fxaa);
        if (ImGui::Checkbox("FXAA##DrawPostProcessPanel", &Enabled))
        {
            mRenderer->SetPostProcessEffectEnabled(PostProcessEffectType::Fxaa, Enabled);
        }

        if (Enabled)
        {
            ImGui::Indent();
            FxaaEffect *pFxaa = mRenderer->GetFxaaEffect();
            ImGui::SliderFloat("Subpixel Quality##DrawPostProcessPanel_Fxaa", &pFxaa->GetSubpixelQuality_NonConst(), 0.0f, 1.0f);
            ImGui::SliderFloat("Edge Threshold##DrawPostProcessPanel_Fxaa", &pFxaa->GetEdgeThreshold_NonConst(), 0.063f, 0.333f);
            ImGui::SliderFloat("Edge Threshold Min##DrawPostProcessPanel_Fxaa", &pFxaa->GetEdgeThresholdMin_NonConst(), 0.0f, 0.0833f);
            ImGui::Unindent();
        }
    }

    ImGui::Separator();

    // ── Chromatic Aberration ───────────────────────────────────────────────────
    {
        bool Enabled = mRenderer->GetPostProcessEffectEnabled(PostProcessEffectType::ChromaticAberration);
        if (ImGui::Checkbox("Chromatic Aberration##DrawPostProcessPanel", &Enabled))
        {
            mRenderer->SetPostProcessEffectEnabled(PostProcessEffectType::ChromaticAberration, Enabled);
        }

        if (Enabled)
        {
            ImGui::Indent();
            ChromaticAberrationEffect *pCA = mRenderer->GetChromaticAberrationEffect();
            ImGui::SliderFloat("Strength##DrawPostProcessPanel_CA", &pCA->GetStrength_NonConst(), 0.0f, 0.02f, "%.4f");
            ImGui::Unindent();
        }
    }

    ImGui::Separator();

    // ── Lens Distortion ───────────────────────────────────────────────────────
    {
        bool Enabled = mRenderer->GetPostProcessEffectEnabled(PostProcessEffectType::LensDistortion);
        if (ImGui::Checkbox("Lens Distortion##DrawPostProcessPanel", &Enabled))
        {
            mRenderer->SetPostProcessEffectEnabled(PostProcessEffectType::LensDistortion, Enabled);
        }

        if (Enabled)
        {
            ImGui::Indent();
            LensDistortionEffect *pLD = mRenderer->GetLensDistortionEffect();
            ImGui::SliderFloat("Barrel##DrawPostProcessPanel_LD", &pLD->GetBarrel_NonConst(), -0.5f, 0.5f);
            ImGui::SliderFloat("Zoom##DrawPostProcessPanel_LD", &pLD->GetZoom_NonConst(), 0.5f, 1.0f);
            ImGui::Unindent();
        }
    }

    ImGui::Separator();

    // ── Cinematic ─────────────────────────────────────────────────────────────
    {
        bool Enabled = mRenderer->GetPostProcessEffectEnabled(PostProcessEffectType::Cinematic);
        if (ImGui::Checkbox("Cinematic##DrawPostProcessPanel", &Enabled))
        {
            mRenderer->SetPostProcessEffectEnabled(PostProcessEffectType::Cinematic, Enabled);
        }

        if (Enabled)
        {
            ImGui::Indent();
            CinematicEffect *pCin = mRenderer->GetCinematicEffect();
            ImGui::SliderFloat("Vignette Radius##DrawPostProcessPanel_Cin", &pCin->GetVignetteRadius_NonConst(), 0.0f, 2.0f);
            ImGui::SliderFloat("Vignette Softness##DrawPostProcessPanel_Cin", &pCin->GetVignetteSoftness_NonConst(), 0.0f, 1.0f);
            ImGui::SliderFloat("Grain Strength##DrawPostProcessPanel_Cin", &pCin->GetGrainStrength_NonConst(), 0.0f, 0.5f);
            ImGui::Unindent();
        }
    }
}

void Canavar::Engine::ImGuiWidget::DrawRendererProperties()
{
    if (ImGui::CollapsingHeader("Renderer##DrawRendererProperties"))
    {
        ImGui::Checkbox("Render Bounding Boxes##DrawRendererProperties", &mRenderer->GetBoundingBoxRenderer()->GetRenderBoundingBoxes_NonConst());
    }
}

void Canavar::Engine::ImGuiWidget::SetSelectedNode(Node *pNode)
{
    Gizmo *pGizmo = mRenderer->GetGizmo();

    // If the selected node is changing, exit the gizmo mode for the previous node.
    // If gizmo is not active for the previous node, this call is harmless.
    pGizmo->Exit();

    // Update the selected node and copy its name into the buffer for display in the UI.
    mSelectedNode = pNode;

    if (mSelectedNode)
    {
        UpdateNameBuffer();

        // If the selected node is a TexturedModel, enter the gizmo mode for it.
        if (TexturedModel *pTexturedModel = dynamic_cast<TexturedModel *>(mSelectedNode))
        {
            pGizmo->Enter(pTexturedModel);
        }
    }
}

void Canavar::Engine::ImGuiWidget::UpdateNameBuffer()
{
    const auto &Name = mSelectedNode->GetNodeName();
    std::strncpy(mNodeNameBuffer, Name.c_str(), sizeof(mNodeNameBuffer) - 1);
    mNodeNameBuffer[sizeof(mNodeNameBuffer) - 1] = '\0';
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
    const bool StillAlive = std::any_of(Nodes.begin(), Nodes.end(), [this](const NodePtr &pNode) { return pNode.get() == mSelectedNode; });
    if (!StillAlive)
    {
        SetSelectedNode(nullptr);
    }
}
