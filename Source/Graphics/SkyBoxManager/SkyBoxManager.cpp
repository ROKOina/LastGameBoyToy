#include "SkyBoxManager.h"
#include "Graphics/Graphics.h"
#include "Graphics/Shaders/Texture.h"
#include "Graphics/Shaders/FullScreenQuad.h"

SkyBoxManager::SkyBoxManager()
{
  //スカイマップ用ピクセルシェーダー
  CreatePsFromCso(Graphics::Instance().GetDevice(), "Shader\\SkyBoxPS.cso", skyBoxShader.ReleaseAndGetAddressOf());
}

void SkyBoxManager::LoadSkyBoxTextures(std::array<const char*, 4> filepath)
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  // テクスチャ読み込み
  D3D11_TEXTURE2D_DESC texture2d_desc{};
  for (int i = 0; i < 4; ++i) {
    LoadTextureFromFile(device, filepath[i], skyBoxTextures[i].GetAddressOf(), &texture2d_desc);
  }
}

void SkyBoxManager::BindTextures(ID3D11DeviceContext* dc, int startRegisterIndex)
{
  dc->PSSetShaderResources(startRegisterIndex, 4, skyBoxTextures[0].GetAddressOf());
}

void SkyBoxManager::DrawSkyBox(ID3D11DeviceContext* dc)
{
  Graphics& graphics = Graphics::Instance();

  dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
  dc->RSSetState(graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

  FullScreenQuad::Instance().Blit(dc, skyBoxTextures[0].GetAddressOf(), 10, 1, skyBoxShader.Get());
}
