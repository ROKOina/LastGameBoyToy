#include <stdlib.h>
#include <filesystem>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <map>

#include "Texture.h"
#include "Graphics/Graphics.h"
#include "Misc.h"
#include "Logger.h"

//DDSTexture読み込み
static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;
HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
  HRESULT hr{ S_OK };
  Microsoft::WRL::ComPtr<ID3D11Resource> resource;

  auto it = resources.find(filename);
  if (it != resources.end())
  {
    *shader_resource_view = it->second.Get();
    (*shader_resource_view)->AddRef();
    (*shader_resource_view)->GetResource(resource.GetAddressOf());
  }
  else
  {
    std::filesystem::path dds_filename(filename);
    dds_filename.replace_extension("dds");
    if (std::filesystem::exists(dds_filename.c_str()))
    {
      hr = DirectX::CreateDDSTextureFromFile(device, dds_filename.c_str(), resource.GetAddressOf(), shader_resource_view);
      _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
    else
    {
      hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
      _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
    resources.insert(std::make_pair(filename, *shader_resource_view));
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
  hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  texture2d->GetDesc(texture2d_desc);

  return hr;
}

//解放処理
void ReleaseAllTextures()
{
  resources.clear();
}

HRESULT LoadTexture(const char* filename, ID3D11Device* device, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* textureDesc)
{
  // マルチバイト文字からワイド文字へ変換
  std::filesystem::path filepath(filename);
  Microsoft::WRL::ComPtr<ID3D11Resource> resource;
  HRESULT hr;

  // DDSの読み込み
  if (filepath.extension().string() == ".DDS" ||
    filepath.extension().string() == ".dds")
  {
    hr = DirectX::CreateDDSTextureFromFile(device, filepath.c_str(), resource.GetAddressOf(), srv);
  }
  // DDS以外のテクスチャ読み込み
  else
  {
    hr = DirectX::CreateWICTextureFromFile(device, filepath.c_str(), resource.GetAddressOf(), srv);
  }
  // D3D11_TEXTURE2D_DESCの作成
  if (textureDesc && SUCCEEDED(hr))
  {
    Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture2d;
    HRESULT hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    texture2d->GetDesc(textureDesc);
  }

  return hr;
}


