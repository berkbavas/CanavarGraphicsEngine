#include "Model.h"

Canavar::Engine::Model::Model(const QString& modelName)
    : mModelName(modelName)
{
    SetNodeName(mModelName);
}