#include "PrimitiveModel.h"

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

Canavar::Engine::PrimitiveModel::PrimitiveModel(PrimitiveType Type)
    : mPrimitiveType(Type)
{}

void Canavar::Engine::PrimitiveModel::Render(GeometryBase *pGeometry, Shader *pShader, PerspectiveCamera *pCamera)
{
    const auto &M = GetTransformation();

    pShader->Bind();
    pShader->SetUniform("uModelMatrix", M);
    pShader->SetUniform("uNormalMatrix", M.normalMatrix());
    pShader->SetUniform("uColor", mColor);
    pShader->SetUniform("uOpacity", mOpacity);
    pShader->SetUniform("uNodeId", GetNodeId());
    pGeometry->Render();
    pShader->Unbind();
}

bool Canavar::Engine::PrimitiveModel::HasTransparency() const
{
    return mOpacity < 1.0f;
}

bool Canavar::Engine::PrimitiveModel::IsFullyOpaque() const
{
    return mOpacity >= 1.0f;
}

const char *Canavar::Engine::PrimitiveModel::GetNodeTypeName() const
{
    switch (mPrimitiveType)
    {
    case PrimitiveType::Circle:
        return "Circle";
    case PrimitiveType::Disk:
        return "Disk";
    case PrimitiveType::Line:
        return "Line";
    case PrimitiveType::Plane:
        return "Plane";
    case PrimitiveType::Sphere:
        return "Sphere";
    default:
        return "PrimitiveModel";
    }
}
