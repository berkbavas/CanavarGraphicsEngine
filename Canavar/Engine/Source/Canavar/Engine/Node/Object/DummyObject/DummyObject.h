#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

namespace Canavar::Engine
{
    class DummyObject : public Object
    {
        REGISTER_OBJECT_TYPE(DummyObject);

      public:
        DummyObject();

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object) override;
    };

    using DummyObjectPtr = std::shared_ptr<DummyObject>;
}