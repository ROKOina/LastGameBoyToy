#include <stdlib.h>
#include <fstream>
#include <functional>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <filesystem>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "SystemStruct/Misc.h"
#include "SystemStruct/Logger.h"
#include "ModelResource.h"

// CEREALバージョン定義
CEREAL_CLASS_VERSION(ModelResource::Node, 1)
CEREAL_CLASS_VERSION(ModelResource::Material, 1)
CEREAL_CLASS_VERSION(ModelResource::Subset, 1)
CEREAL_CLASS_VERSION(ModelResource::Vertex, 1)
CEREAL_CLASS_VERSION(ModelResource::Mesh, 1)
CEREAL_CLASS_VERSION(ModelResource::NodeKeyData, 1)
CEREAL_CLASS_VERSION(ModelResource::RootPosition, 1)
CEREAL_CLASS_VERSION(ModelResource::Keyframe, 1)
CEREAL_CLASS_VERSION(ModelResource::AnimationEvent, 1)
CEREAL_CLASS_VERSION(ModelResource::Animation, 1)
CEREAL_CLASS_VERSION(ModelResource, 1)

// シリアライズ
namespace DirectX
{
  template<class Archive>
  void serialize(Archive& archive, XMUINT4& v)
  {
    archive(
      cereal::make_nvp("x", v.x),
      cereal::make_nvp("y", v.y),
      cereal::make_nvp("z", v.z),
      cereal::make_nvp("w", v.w)
    );
  }

  template<class Archive>
  void serialize(Archive& archive, XMFLOAT2& v)
  {
    archive(
      cereal::make_nvp("x", v.x),
      cereal::make_nvp("y", v.y)
    );
  }

  template<class Archive>
  void serialize(Archive& archive, XMFLOAT3& v)
  {
    archive(
      cereal::make_nvp("x", v.x),
      cereal::make_nvp("y", v.y),
      cereal::make_nvp("z", v.z)
    );
  }

  template<class Archive>
  void serialize(Archive& archive, XMFLOAT4& v)
  {
    archive(
      cereal::make_nvp("x", v.x),
      cereal::make_nvp("y", v.y),
      cereal::make_nvp("z", v.z),
      cereal::make_nvp("w", v.w)
    );
  }

  template<class Archive>
  void serialize(Archive& archive, XMFLOAT4X4& m)
  {
    archive(
      cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
      cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
      cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
      cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
    );
  }
}

//ノード
template<class Archive>
void ModelResource::Node::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(id),
    CEREAL_NVP(name),
    CEREAL_NVP(path),
    CEREAL_NVP(parentIndex),
    CEREAL_NVP(scale),
    CEREAL_NVP(rotate),
    CEREAL_NVP(translate),
    CEREAL_NVP(layer)
  );
}

//マテリアル
template<class Archive>
void ModelResource::Material::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(name),
    CEREAL_NVP(textureFilename),
    CEREAL_NVP(color),
    CEREAL_NVP(emissivecolor),
    CEREAL_NVP(emissiveintensity),
    CEREAL_NVP(Metalness),
    CEREAL_NVP(Roughness)
  );
}

//サブセット
template<class Archive>
void ModelResource::Subset::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(startIndex),
    CEREAL_NVP(indexCount),
    CEREAL_NVP(materialIndex)
  );
}

template<class Archive>
void ModelResource::Vertex::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(position),
    CEREAL_NVP(normal),
    CEREAL_NVP(tangent),
    CEREAL_NVP(texcoord),
    CEREAL_NVP(color),
    CEREAL_NVP(boneWeight),
    CEREAL_NVP(boneIndex)
  );
}

template<class Archive>
void ModelResource::Mesh::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(vertices),
    CEREAL_NVP(indices),
    CEREAL_NVP(subsets),
    CEREAL_NVP(nodeIndex),
    CEREAL_NVP(nodeIndices),
    CEREAL_NVP(offsetTransforms),
    CEREAL_NVP(boundsMin),
    CEREAL_NVP(boundsMax)
  );
}

template<class Archive>
void ModelResource::NodeKeyData::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(scale),
    CEREAL_NVP(rotate),
    CEREAL_NVP(translate)
  );
}

template<class Archive>
void ModelResource::Keyframe::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(seconds),
    CEREAL_NVP(nodeKeys)
  );
}

template<class Archive>
void ModelResource::RootPosition::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(frame),
    CEREAL_NVP(position)
  );
}

template<class Archive>
void ModelResource::AnimationEvent::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(name),
    CEREAL_NVP(startframe),
    CEREAL_NVP(endframe)
  );
}

template<class Archive>
void ModelResource::Animation::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(name),
    CEREAL_NVP(secondsLength),
    CEREAL_NVP(animationspeed),
    CEREAL_NVP(keyframes),
    CEREAL_NVP(animationevents),
    CEREAL_NVP(rootpositions)
  );
}

// 読み込み
void ModelResource::Load(ID3D11Device* device, const char* filename)
{
  // ディレクトリパス取得
  char drive[32], dir[256], dirname[256];
  ::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
  ::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

  // デシリアライズ
  Deserialize(filename);

  //マテリアルデシリアライズ
  MaterialDeserialize(filename, materials_);

  //アニメーションデシリアライズ
  AnimationDeserialize(filename);

  //ノードデシリアライズ
  NodeDeserialize(filename);

  // モデル構築
  BuildModel(device, dirname);
}

void ModelResource::LoadMaterial(ID3D11Device* device, const char* filename, std::shared_ptr<std::vector<ModelResource::Material>> desc)
{
  // ディレクトリパス取得
  char drive[32], dir[256], dirname[256];
  ::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
  ::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

  //マテリアルデシリアライズ
  MaterialDeserialize(filename, *desc.get());

  // テクスチャ読み込み
  for (Material& material : *desc.get())
  {
    // 相対パスの解決
    char filename[256];
    for (int i = 0; i < 6; ++i)
    {
      ::_makepath_s(filename, 256, nullptr, dirname, material.textureFilename[i].c_str(), nullptr);

      material.LoadTexture(device, filename, i);
    }
  }
}

#ifdef _DEBUG

//マテリアルシリアライズ
void ModelResource::MaterialSerialize(const char* filename)
{
  std::string name = filename;
  name.erase(name.find('.') + 1);
  name += "Material";

  std::ofstream ostream(name, std::ios::binary);
  if (ostream.is_open())
  {
    cereal::BinaryOutputArchive archive(ostream);

    try
    {
      archive
      (
        CEREAL_NVP(materials_)
      );
    }
    catch (...)
    {
      LOG("model deserialize materialfailed.\n%s\n", filename);
      return;
    }
  }
}

#endif // _DEBUG

// モデル構築
void ModelResource::BuildModel(ID3D11Device* device, const char* dirname)
{
  for (Material& material : materials_)
  {
    // 相対パスの解決
    char filename[256];
    for (int i = 0; i < 6; ++i)
    {
      ::_makepath_s(filename, 256, nullptr, dirname, material.textureFilename[i].c_str(), nullptr);

      // テクスチャ読み込み
      material.LoadTexture(device, filename, i);
    }
  }

  for (Mesh& mesh : meshes_)
  {
    // 頂点バッファ
    {
      D3D11_BUFFER_DESC bufferDesc = {};
      D3D11_SUBRESOURCE_DATA subresourceData = {};

      bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
      //bufferDesc.Usage = D3D11_USAGE_DEFAULT;
      bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
      bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      bufferDesc.CPUAccessFlags = 0;
      bufferDesc.MiscFlags = 0;
      bufferDesc.StructureByteStride = 0;
      subresourceData.pSysMem = mesh.vertices.data();
      subresourceData.SysMemPitch = 0;
      subresourceData.SysMemSlicePitch = 0;

      HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // インデックスバッファ
    {
      D3D11_BUFFER_DESC bufferDesc = {};
      D3D11_SUBRESOURCE_DATA subresourceData = {};

      bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * mesh.indices.size());
      //bufferDesc.Usage = D3D11_USAGE_DEFAULT;
      bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
      bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      bufferDesc.CPUAccessFlags = 0;
      bufferDesc.MiscFlags = 0;
      bufferDesc.StructureByteStride = 0;
      subresourceData.pSysMem = mesh.indices.data();
      subresourceData.SysMemPitch = 0; //Not use for index buffers.
      subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
      HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
  }
}

// テクスチャ読み込み
void ModelResource::Material::LoadTexture(ID3D11Device* device, const char* filename, int number)
{
  // マルチバイト文字からワイド文字へ変換
  wchar_t wfilename[256];
  ::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, 256);

  // テクスチャ読み込み
  Microsoft::WRL::ComPtr<ID3D11Resource> resource;
  HRESULT hr = DirectX::CreateDDSTextureFromFile(device, wfilename, resource.GetAddressOf(), shaderResourceView[number].ReleaseAndGetAddressOf());
  if (FAILED(hr))
  {
    // WICでサポートされていないフォーマットの場合（TGAなど）は
    // STBで画像読み込みをしてテクスチャを生成する
    int width, height, bpp;
    unsigned char* pixels = stbi_load(filename, &width, &height, &bpp, STBI_rgb_alpha);
    if (pixels != nullptr)
    {
      D3D11_TEXTURE2D_DESC desc = { 0 };
      desc.Width = width;
      desc.Height = height;
      desc.MipLevels = 1;
      desc.ArraySize = 1;
      desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      desc.SampleDesc.Count = 1;
      desc.SampleDesc.Quality = 0;
      desc.Usage = D3D11_USAGE_DEFAULT;
      desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      desc.CPUAccessFlags = 0;
      desc.MiscFlags = 0;
      D3D11_SUBRESOURCE_DATA data;
      ::memset(&data, 0, sizeof(data));
      data.pSysMem = pixels;
      data.SysMemPitch = width * 4;

      Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
      HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

      hr = device->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView[number].ReleaseAndGetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

      // 後始末
      stbi_image_free(pixels);
    }
    else
    {
      // 読み込み失敗したらダミーテクスチャを作る
      LOG("load failed : %s\n", filename);

      const int width = 8;
      const int height = 8;
      UINT pixels[width * height];
      ::memset(pixels, 0xFF, sizeof(pixels));

      D3D11_TEXTURE2D_DESC desc = { 0 };
      desc.Width = width;
      desc.Height = height;
      desc.MipLevels = 1;
      desc.ArraySize = 1;
      desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      desc.SampleDesc.Count = 1;
      desc.SampleDesc.Quality = 0;
      desc.Usage = D3D11_USAGE_DEFAULT;
      desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      desc.CPUAccessFlags = 0;
      desc.MiscFlags = 0;
      D3D11_SUBRESOURCE_DATA data;
      ::memset(&data, 0, sizeof(data));
      data.pSysMem = pixels;
      data.SysMemPitch = width;

      Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
      HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

      hr = device->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView[number].ReleaseAndGetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
  }
}

// シリアライズ
void ModelResource::Serialize(const char* filename)
{
  std::ofstream ostream(filename, std::ios::binary);
  if (ostream.is_open())
  {
    cereal::BinaryOutputArchive archive(ostream);

    try
    {
      archive(
        CEREAL_NVP(nodes_),
        CEREAL_NVP(materials_),
        CEREAL_NVP(meshes_),
        CEREAL_NVP(animations_)
      );
    }
    catch (...)
    {
      LOG("model deserialize failed.\n%s\n", filename);
      return;
    }
  }
}

void ModelResource::AnimSerialize()
{
  std::string name = fileName;
  name.erase(name.find('.') + 1);
  name += "Animation";

  std::ofstream ostream(name, std::ios::binary);
  if (ostream.is_open())
  {
    cereal::BinaryOutputArchive archive(ostream);

    try
    {
      archive(
        CEREAL_NVP(animations_)
      );
    }
    catch (...)
    {
      LOG("model deserialize failed.\n%s\n", fileName);
      return;
    }
  }

}

// デシリアライズ
void ModelResource::Deserialize(const char* filename)
{
  fileName = filename;

  std::ifstream istream(filename, std::ios::binary);
  if (istream.is_open())
  {
    cereal::BinaryInputArchive archive(istream);

    try
    {
      archive(
        CEREAL_NVP(meshes_)
      );
    }
    catch (...)
    {
      LOG("model deserialize failed.\n%s\n", filename);
      return;
    }
  }
}

//マテリアルデシリアライズ
void ModelResource::MaterialDeserialize(const char* filename, std::vector<Material>&	materials)
{
  std::string name = filename;
  name.erase(name.find('.') + 1);
  name += "Material";

  std::ifstream istream(name, std::ios::binary);
  if (istream.is_open())
  {
    cereal::BinaryInputArchive archive(istream);

    try
    {
      archive
      (
        CEREAL_NVP(materials)
      );
    }
    catch (...)
    {
      LOG("model deserialize materialfailed.\n%s\n", filename);
      return;
    }
  }
}

//アニメーションデシリアライズ
void ModelResource::AnimationDeserialize(const char* filename)
{
  std::string name = filename;
  name.erase(name.find('.') + 1);
  name += "Animation";

  std::ifstream istream(name, std::ios::binary);
  if (istream.is_open())
  {
    cereal::BinaryInputArchive archive(istream);

    try
    {
      archive
      (
        CEREAL_NVP(animations_)
      );
    }
    catch (...)
    {
      LOG("model deserialize animationfailed.\n%s\n", filename);
      return;
    }
  }
}

//ノードデシリアライズ
void ModelResource::NodeDeserialize(const char* filename)
{
  std::string name = filename;
  name.erase(name.find('.') + 1);
  name += "Node";

  std::ifstream istream(name, std::ios::binary);
  if (istream.is_open())
  {
    cereal::BinaryInputArchive archive(istream);

    try
    {
      archive
      (
        CEREAL_NVP(nodes_)
      );
    }
    catch (...)
    {
      LOG("model deserialize nodefailed.\n%s\n", filename);
      return;
    }
  }
}

// ノードインデックスを取得する
int ModelResource::FindNodeIndex(NodeId nodeId) const
{
  int nodeCount = static_cast<int>(nodes_.size());
  for (int i = 0; i < nodeCount; ++i)
  {
    if (nodes_[i].id == nodeId)
    {
      return i;
    }
  }
  return -1;
}