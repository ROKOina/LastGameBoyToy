#include "Graphics\Graphics.h"
#include "ResourceManager.h"
#include <map>

//���f�����\�[�X�ǂݍ���
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
  std::shared_ptr<ModelResource> model;

  //���f������
  for (auto& modelmap : models_) {
    if (modelmap.first == filename) {
      if (modelmap.second) {
        model = std::make_shared<ModelResource>(*modelmap.second.get());
      }
    }
  }

  //���f�����Ȃ��ꍇ
  if (!model) {
    //�V�K���f�����\�[�X�쐬���ǂݍ���
    std::shared_ptr<ModelResource> resource = std::make_shared<ModelResource>();
    resource->Load(Graphics::Instance().GetDevice(), filename);

    //�}�b�v�ɓo�^
    models_[filename] = resource;
    model = static_cast<std::shared_ptr<ModelResource>>(models_[filename]);
  }

  return model;
}

std::shared_ptr<std::vector<ModelResource::Material>> ResourceManager::LoadMaterialResource(const char* filename)
{
  std::shared_ptr<std::vector<ModelResource::Material>> resource;

  //���f������
  for (auto& modelmap : material_) {
    if (modelmap.first == filename) {
      if (modelmap.second) {
        resource = std::make_shared<std::vector<ModelResource::Material>>(*modelmap.second.get());
      }
    }
  }

  //���f�����Ȃ��ꍇ
  if (!resource) {
    //�V�K���f�����\�[�X�쐬���ǂݍ���
    std::shared_ptr<std::vector<ModelResource::Material>> material = std::make_shared<std::vector<ModelResource::Material>>();
    ModelResource::LoadMaterial(Graphics::Instance().GetDevice(), filename, material);

    //�}�b�v�ɓo�^
    material_[filename] = material;
    resource = material_[filename];
  }

  return resource;
}

void ResourceManager::RegisterModel(const char* filename, std::shared_ptr<ModelResource> resource)
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (models_.count(filename) == 0)
    models_[filename] = resource;
}

void ResourceManager::RegisterMaterial(const char* filename, std::shared_ptr<std::vector<ModelResource::Material>> material)
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (material_.count(filename) == 0)
    material_[filename] = material;
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

bool ResourceManager::JudgeMaterialFilename(const char* filename)
{
  if (material_.count(filename) == 0)
    return false;
  else
  {
    if (material_[filename])
      return true;
    else
      return false;
  }
}

//std::shared_ptr<ParticleSystemCom::SaveParticleData> ResourceManagerParticle::LoadParticleResource(const char* filename)
//{
//    std::lock_guard<std::mutex> lock(mutex_);
//
//    std::shared_ptr<ParticleSystemCom::SaveParticleData> particle;
//
//    //���f������
//    for (auto& particlemap : particleMap_) {
//        if (particlemap.first == filename) {
//            if (particlemap.second) {
//                particle = std::make_shared<ParticleSystemCom::SaveParticleData>(*particlemap.second.get());
//            }
//        }
//    }
//
//    //���f�����Ȃ��ꍇ
//    if (!particle) {
//        //�V�K���f�����\�[�X�쐬���ǂݍ���
//        ParticleSystemCom* particleCom=new ParticleSystemCom(1000);
//        std::shared_ptr<ParticleSystemCom::SaveParticleData> resource = std::make_shared<ParticleSystemCom::SaveParticleData>(particleCom->LoadParticle(filename));
//
//        //�}�b�v�ɓo�^
//        particleMap_[filename] = resource;
//        particle = static_cast<std::shared_ptr<ParticleSystemCom::SaveParticleData>>(particleMap_[filename]);
//
//        delete particleCom;
//    }
//
//    return particle;
//}