#pragma once

#include "Canavar/Engine/Node/Object/Camera/PerspectiveCamera.h"

namespace Canavar::Engine
{
    class DummyCamera : public PerspectiveCamera
    {
        REGISTER_NODE_TYPE(DummyCamera);

      public:
        DummyCamera();

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes) override;
    };

    using DummyCameraPtr = std::shared_ptr<DummyCamera>;
}