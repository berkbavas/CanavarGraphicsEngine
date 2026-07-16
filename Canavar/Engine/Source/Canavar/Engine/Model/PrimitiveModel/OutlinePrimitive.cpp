#include "OutlinePrimitive.h"

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

Canavar::Engine::OutlinePrimitive::OutlinePrimitive(PrimitiveType Type)
    : PrimitiveModel(Type)
{}

void Canavar::Engine::OutlinePrimitive::Render(GeometryBase *pGeometry, Shader *pShader, PerspectiveCamera *pCamera)
{
    pShader->Bind();
    pShader->SetUniform("uModelMatrix", GetTransformation());
    pShader->SetUniform("uColor", mColor);
    pShader->SetUniform("uOpacity", mOpacity);
    pShader->SetUniform("uThickness", mThickness);
    pShader->SetUniform("uNodeId", GetNodeId());
    pGeometry->Render();
    pShader->Unbind();
}
