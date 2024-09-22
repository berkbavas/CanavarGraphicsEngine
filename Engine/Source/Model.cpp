#include "Model.h"
#include "ModelData.h"
#include "ModelDataManager.h"

Canavar::Engine::Model::Model(const QString& modelName)
    : Node()
    , mModelName(modelName)
    , mData(nullptr)
    , mColor(1, 1, 1, 1)
    , mOverlayColor(1, 1, 1, 1)
    , mOverlayColorFactor(0.0f)
    , mAmbient(0.3f)
    , mDiffuse(0.75)
    , mSpecular(0.25)
    , mShininess(32.0f)
{
    mName = modelName;
    mType = Node::NodeType::Model;
    mData = ModelDataManager::Instance()->GetModelData(mModelName);

    if (mData)
        SetAABB(mData->GetAABB());
}

QMatrix4x4 Canavar::Engine::Model::GetMeshTransformation(const QString& meshName)
{
    return mMeshTransformations.value(meshName, QMatrix4x4());
}

void Canavar::Engine::Model::SetMeshTransformation(const QString& meshName, const QMatrix4x4& transformation)
{
    mMeshTransformations.insert(meshName, transformation);

    UpdateAABB();
}

QVector4D Canavar::Engine::Model::GetMeshOverlayColor(const QString& meshName)
{
    return mMeshOverlayColors.value(meshName, QVector4D(0, 0, 0, 0));
}

void Canavar::Engine::Model::SetMeshOverlayColor(const QString& meshName, const QVector4D& color)
{
    mMeshOverlayColors.insert(meshName, color);
}

float Canavar::Engine::Model::GetMeshOverlayColorFactor(const QString& meshName)
{
    return mMeshOverlayColorFactors.value(meshName, 0);
}

void Canavar::Engine::Model::SetMeshOverlayColorFactor(const QString& meshName, float factor)
{
    mMeshOverlayColorFactors.insert(meshName, factor);
}

void Canavar::Engine::Model::Render(RenderMode renderMode)
{
    if (mData)
        mData->Render(renderMode, this);
}

void Canavar::Engine::Model::UpdateAABB()
{
    if (mData)
    {
        float inf = std::numeric_limits<float>::infinity();
        QVector3D min(inf, inf, inf);
        QVector3D max(-inf, -inf, -inf);

        for (const auto& mesh : mData->GetMeshes())
        {
            auto aabb = mesh->GetAABB().Transform(mMeshTransformations.value(mesh->GetName()));

            if (min[0] > aabb.GetMin().x())
                min[0] = aabb.GetMin().x();

            if (min[1] > aabb.GetMin().y())
                min[1] = aabb.GetMin().y();

            if (min[2] > aabb.GetMin().z())
                min[2] = aabb.GetMin().z();

            if (max[0] < aabb.GetMax().x())
                max[0] = aabb.GetMax().x();

            if (max[1] < aabb.GetMax().y())
                max[1] = aabb.GetMax().y();

            if (max[2] < aabb.GetMax().z())
                max[2] = aabb.GetMax().z();
        }

        mAABB.SetMin(min);
        mAABB.SetMax(max);
    }
}

void Canavar::Engine::Model::ToJson(QJsonObject& object)
{
    Node::ToJson(object);

    QJsonObject color;
    color.insert("r", mColor.x());
    color.insert("g", mColor.y());
    color.insert("b", mColor.z());
    color.insert("a", mColor.w());
    object.insert("color", color);

    QJsonObject overlayColor;
    overlayColor.insert("r", mOverlayColor.x());
    overlayColor.insert("g", mOverlayColor.y());
    overlayColor.insert("b", mOverlayColor.z());
    overlayColor.insert("a", mOverlayColor.w());
    object.insert("overlay_color", overlayColor);

    object.insert("overlay_color_factor", mOverlayColorFactor);
    object.insert("ambient", mAmbient);
    object.insert("diffuse", mDiffuse);
    object.insert("specular", mSpecular);
    object.insert("shininess", mShininess);
    object.insert("model_name", mModelName);

    // TODO
    // mMeshTransformations, mMeshOverlayColors, mMeshOverlayColorFactors
}

void Canavar::Engine::Model::FromJson(const QJsonObject& object)
{
    Node::FromJson(object);

    // Color
    {
        float r = object["color"]["r"].toDouble();
        float g = object["color"]["g"].toDouble();
        float b = object["color"]["b"].toDouble();
        float a = object["color"]["a"].toDouble();

        mColor = QVector4D(r, g, b, a);
    }

    // Overlay Color
    {
        float r = object["overlay_color"]["r"].toDouble();
        float g = object["overlay_color"]["g"].toDouble();
        float b = object["overlay_color"]["b"].toDouble();
        float a = object["overlay_color"]["a"].toDouble();

        mOverlayColor = QVector4D(r, g, b, a);
    }

    mOverlayColorFactor = object["overlay_color_factor"].toDouble();
    mAmbient = object["ambient"].toDouble();
    mDiffuse = object["diffuse"].toDouble();
    mSpecular = object["specular"].toDouble();
    mShininess = object["shininess"].toDouble();
    mModelName = object["model_name"].toString();
}