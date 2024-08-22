#include "Graphics\Graphics.h"
#include "ResourceManager.h"
#include <map>

//モデルリソース読み込み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
    std::shared_ptr<ModelResource> model;

    //モデル検索
    for (auto& modelmap : models_) {
        if (modelmap.first == filename) {
            if (modelmap.second) {
                model = std::make_shared<ModelResource>(*modelmap.second.get());
            }
        }
    }

    //モデルがない場合
    if (!model) {
        //新規モデルリソース作成＆読み込み
        std::shared_ptr<ModelResource> resource = std::make_shared<ModelResource>();
        resource->Load(Graphics::Instance().GetDevice(), filename);

        //マップに登録
        models_[filename] = resource;
        model = static_cast<std::shared_ptr<ModelResource>>(models_[filename]);
    }

    return model;
}

void ResourceManager::RegisterModel(const char* filename, std::shared_ptr<ModelResource> resource)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (models_.count(filename) == 0)
        models_[filename] = resource;
}

bool ResourceManager::JudgeModelFilename(const char* filename)
{
    if (models_.count(filename) == 0)
        return false;
    else
    {
        if (models_[filename])
            return true;
        else
            return false;
    }
}

