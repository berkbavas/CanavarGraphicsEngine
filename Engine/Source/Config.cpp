#include "Config.h"
#include "Helper.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Canavar::Engine::Config::Config(QObject *parent)
    : QObject(parent)
    , mNodeSelectionEnabled(false)
{}

Canavar::Engine::Config *Canavar::Engine::Config::Instance()
{
    static Config instance;
    return &instance;
}

void Canavar::Engine::Config::Load(const QString &configFile)
{
    QJsonDocument doc = Helper::LoadJson(configFile);

    if (doc.isNull() || doc.isEmpty()) // Use default values
    {
        mModelsRootFolder = "Resources/Models";
        mSupportedModelFormats << "*.obj"
                               << "*.blend"
                               << "*.fbx";

        return;
    }

    auto object = doc.object();

    mModelsRootFolder = object.value("models_root_folder").toString(mModelsRootFolder);
    mNodeSelectionEnabled = object.value("node_selection_enabled").toBool();
    mWorldFilePath = object.value("world_file_path").toString();

    auto formats = object.value("model_formats").toArray();

    for (const auto &format : formats)
        mSupportedModelFormats << format.toString();
}
