#pragma once

#include "Manager.h"
#include "ModelData.h"

#include <QMap>

namespace Canavar {
    namespace Engine {
        class ModelDataManager : public Manager
        {
        private:
            ModelDataManager();

        public:
            static ModelDataManager* Instance();

            bool Init();

            ModelData* GetModelData(const QString& modelName);
            const QStringList& GetModelNames() const;

        private:
            void LoadModels(const QString& path, const QStringList& formats);

        private:
            QMap<QString, ModelData*> mModelsData;
            QStringList mModelNames;
        };
    } // namespace Engine
} // namespace Canavar