#pragma once

#include <wrl.h>
#include <d3d11.h>

// テクスチャ
//DDSTextureを読み込むための処理
HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);

//解放処理
void ReleaseAllTextures();

#ifdef _DEBUG
HRESULT LoadTexture(const char* filename, ID3D11Device* device, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* textureDesc);

#endif // _DEBUG
