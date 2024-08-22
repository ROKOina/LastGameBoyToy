#pragma once

#include <memory>
#include <string>
#include <map>

#include "ModelResource.h"
#include <mutex>

//リソースマネジャー
class ResourceManager
{
private:
    ResourceManager() {}
    ~ResourceManager() {}

public:
    //唯一のインスタンス取得
    static ResourceManager& Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    //モデルリソース読み込み
    std::shared_ptr<ModelResource> LoadModelResource(const char* filename);

    //モデルリソース登録
    void RegisterModel(const char* filename, std::shared_ptr<ModelResource> resource);

    //ファイル名で登録確認
    bool JudgeModelFilename(const char* filename);

private:
    using ModelMap = std::map<std::string, std::shared_ptr<ModelResource>>;

    ModelMap models_;
    std::mutex mutex_;
};
