#pragma once

#include "Common.h"

#include <QObject>

namespace Canavar {
    namespace Engine {
        class Config : public QObject
        {
        private:
            explicit Config(QObject* parent = nullptr);

        public:
            static Config* Instance();

            void Load(const QString& configFile);

            DEFINE_MEMBER_CONST(QString, ModelsRootFolder);
            DEFINE_MEMBER_CONST(QString, WorldFilePath);
            DEFINE_MEMBER_CONST(QStringList, SupportedModelFormats);
            DEFINE_MEMBER_CONST(bool, NodeSelectionEnabled);
        };
    } // namespace Engine
} // namespace Canavar