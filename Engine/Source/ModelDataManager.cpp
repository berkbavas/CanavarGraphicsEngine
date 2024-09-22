#include "ModelDataManager.h"
#include "Config.h"
#include "Helper.h"

#include <QDir>

Canavar::Engine::ModelDataManager::ModelDataManager()
    : Manager()
{}

Canavar::Engine::ModelDataManager *Canavar::Engine::ModelDataManager::Instance()
{
    static ModelDataManager instance;

    return &instance;
}

bool Canavar::Engine::ModelDataManager::Init()
{
    LoadModels(Config::Instance()->GetModelsRootFolder(), Config::Instance()->GetSupportedModelFormats());

    mModelNames = mModelsData.keys();

    return true;
}

Canavar::Engine::ModelData *Canavar::Engine::ModelDataManager::GetModelData(const QString &modelName)
{
    return mModelsData.value(modelName, nullptr);
}

const QStringList &Canavar::Engine::ModelDataManager::GetModelNames() const
{
    return mModelNames;
}

void Canavar::Engine::ModelDataManager::LoadModels(const QString &path, const QStringList &formats)
{
    qInfo() << "Loading and creating all models at" << path << "whose extensions are" << formats;

    QDir dir(path);
    auto dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    for (const auto &dirName : qAsConst(dirs))
    {
        QDir childDir(dir.path() + "/" + dirName);
        childDir.setNameFilters(formats);
        auto files = childDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

        for (const auto &file : qAsConst(files))
        {
            auto path = childDir.path() + "/" + file;

            qInfo() << "Loading model" << dirName << "at" << path;

            ModelData *data = Canavar::Engine::Helper::LoadModel(dirName, path);

            if (data)
            {
                auto meshes = data->GetMeshes();

                for (auto mesh : meshes)
                    mesh->Create();

                mModelsData.insert(data->GetName(), data);

                qInfo() << "Model" << dirName << "at" << path << "is loaded.";
            } else
                qWarning() << "Model" << dirName << "at" << path << "could not be loaded.";
        }
    }

    qInfo() << "All models are loaded at" << path;
}
