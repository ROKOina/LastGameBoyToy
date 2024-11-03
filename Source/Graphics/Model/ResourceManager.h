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

    //マテリアルリソース読み込み
    std::shared_ptr<std::vector<ModelResource::Material>> LoadMaterialResource(const char* filename);

    //モデルリソース登録
    void RegisterModel(const char* filename, std::shared_ptr<ModelResource> resource);

    //マテリアルリソース登録
    void RegisterMaterial(const char* filename, std::shared_ptr<std::vector<ModelResource::Material>> material);

    //ファイル名で登録確認
    bool JudgeModelFilename(const char* filename);

    //ファイル名で登録確認
    bool JudgeMaterialFilename(const char* filename);

private:
    using ModelMap = std::map<std::string, std::shared_ptr<ModelResource>>;
    using MaterialMap = std::map<std::string, std::shared_ptr<std::vector<ModelResource::Material>>>;

    ModelMap models_;
    MaterialMap material_;
    std::mutex mutex_;
};

//パーティクルリソースマネジャー
class ResourceManagerParticle
{
    //private:
    //    ResourceManagerParticle(){}
    //    ~ResourceManagerParticle(){}
    //
    //public:
    //    //唯一のインスタンス取得
    //    static ResourceManagerParticle& Instance()
    //    {
    //        static ResourceManagerParticle instance;
    //        return instance;
    //    }
    //
    //    //パーティクルリソース読み込み
    //    std::shared_ptr<ParticleSystemCom::SaveParticleData> LoadParticleResource(const char* filename);
    //
    //    ////パーティクルリソース登録
    //    //void RegisterParticle(const char* filename, std::shared_ptr<ParticleSystemCom::SaveParticleData> resource);
    //
    //    ////ファイル名で登録確認
    //    //bool JudgeParticleFilename(const char* filename);
    //
    //private:
    //    using ParticleMap = std::map<std::string, std::shared_ptr<ParticleSystemCom::SaveParticleData>>;
    //
    //    ParticleMap particleMap_;
    //    std::mutex mutex_;
};