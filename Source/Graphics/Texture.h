#pragma once

#include "Graphics/Graphics.h"
#include "SystemStruct\Misc.h"
#include <stb_image.h>


// テクスチャ
//DDSTextureを読み込むための処理
HRESULT LoadTextureFromFile(ID3D11Device* device, const char* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);

//HDRTexture読み込み
HRESULT LoadHDRTexture(ID3D11Device* device, const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D11Resource>& resource, ID3D11ShaderResourceView** shader_resource_view);

//解放処理
void ReleaseAllTextures();

#ifdef _DEBUG
HRESULT LoadTexture(const char* filename, ID3D11Device* device, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* textureDesc);

#endif // _DEBUG