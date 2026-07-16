#include "PrimitiveModel.h"

Canavar::Engine::PrimitiveModel::PrimitiveModel(PrimitiveType Type)
    : mPrimitiveType(Type)
{}

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
