#include "PostEffect.h"
#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include <imgui.h>
#include "Shader.h"

//コンストラクタ
PostEffect::PostEffect()
{
  Graphics& Graphics = Graphics::Instance();

  //ブルームセット
  m_bloomeffect = std::make_unique<Bloom>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight());

  //フレームバッファ生成
  m_offScreenBuffer[static_cast<int>(offscreen::offscreen)] = std::make_unique<FrameBuffer>(Graphics.GetDevice(), static_cast<uint32_t>(Graphics.GetScreenWidth()), static_cast<uint32_t>(Graphics.GetScreenHeight()), DXGI_FORMAT_R32G32B32A32_FLOAT, false);
  m_offScreenBuffer[static_cast<int>(offscreen::posteffect)] = std::make_unique<FrameBuffer>(Graphics.GetDevice(), static_cast<uint32_t>(Graphics.GetScreenWidth()), static_cast<uint32_t>(Graphics.GetScreenHeight()), DXGI_FORMAT_R32G32B32A32_FLOAT, false);

  //ピクセルシェーダー
  CreatePsFromCso(Graphics.GetDevice(), "Shader\\DeferredPBR_PS.cso", m_pixelshaders[static_cast<int>(pixelshader::deferred)].GetAddressOf());
  CreatePsFromCso(Graphics.GetDevice(), "Shader\\ColorGradingPS.cso", m_pixelshaders[static_cast<int>(pixelshader::colorGrading)].GetAddressOf());
  CreatePsFromCso(Graphics.GetDevice(), "Shader\\ToneMapPS.cso", m_pixelshaders[static_cast<int>(pixelshader::tonemap)].GetAddressOf());

  //MultiRenderTarget作成
  m_gBuffer = std::make_unique<decltype(m_gBuffer)::element_type>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight(), 5);

  //コンスタントバッファ
  m_posteffect = std::make_unique<ConstantBuffer<POSTEFFECT>>(Graphics.GetDevice());
}

//デファードの最初の処理
void PostEffect::SetDeferredTarget()
{
  Graphics& Graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

  //skymap描画
  SkyBoxManager::Instance().DrawSkyBox(dc);

  //MultiRenderTargetの描画段階
  m_gBuffer->clear(dc);
  m_gBuffer->activate(dc);
}

//デファードの終了
void PostEffect::EndDeferred()
{
  Graphics& Graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

  // Gバッファ用のレンダーターゲットをGPUから解放
  m_gBuffer->LiberationRenderTarget(dc);

  // スカイボックスのリソースをバインド
  SkyBoxManager::Instance().BindTextures(dc, 10);

  // 深度値の書き込みはしない
  dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);

  // Gバッファを元に描画 ( PBR + IBL )
  FullScreenQuad::Instance().Blit(dc, m_gBuffer->GetShaderResources(), 0,
    m_gBuffer->BufferCount(), m_pixelshaders[static_cast<int>(pixelshader::deferred)].Get());

  // 深度ステンシルを戻す
  dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 1);
}

//ポストエフェクト描画
void PostEffect::PostEffectRender()
{
  Graphics& Graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = Graphics.GetDeviceContext();
  const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

  // GバッファのDSVをGPUから解放
  m_gBuffer->LiberationDepthStencil(dc);
  m_gBuffer->ReleaseCache(dc);
  // オフスクリーンへの描画を止める
  m_offScreenBuffer[static_cast<int>(offscreen::offscreen)]->Deactivate(dc);

  //コンスタントバッファのアクティブ
  m_posteffect->Activate(dc, (int)CB_INDEX::POST_EFFECT, true, true, false, false, false, false);

  //ポストエフェクト
  m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->Clear(dc);
  m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->Activate(dc);
  dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
  dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
  dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

  // 色調補正
  FullScreenQuad::Instance().Blit(dc,
    m_offScreenBuffer[static_cast<int>(offscreen::offscreen)]->m_shaderresourceviews[0].GetAddressOf(),
    0, 1, m_pixelshaders[static_cast<int>(pixelshader::colorGrading)].Get());
  m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->Deactivate(dc);

  //ブルーム
  dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
  dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
  dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
  m_bloomeffect->Make(dc, m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->m_shaderresourceviews[0].Get());
  m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->Activate(dc);
  dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::ADD), blendFactor, 0xFFFFFFFF);
  dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
  dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
  m_bloomeffect->Blit(dc);
  m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->Deactivate(dc);

  //トーンマップ
  dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
  dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
  dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
  ID3D11ShaderResourceView* tone[]{ m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->m_shaderresourceviews[0].Get() };
  FullScreenQuad::Instance().Blit(dc, tone, 0, _countof(tone), m_pixelshaders[static_cast<int>(pixelshader::tonemap)].Get());
}

//imgui描画
void PostEffect::PostEffectImGui()
{
  ImGui::Begin("PostEffect");
  //ポストエフェクト
  if (ImGui::CollapsingHeader("PostEffect", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::ColorEdit3("colorize", m_posteffect->data.colorize);
    ImGui::SliderFloat("exposure", &m_posteffect->data.exposure, +0.0f, +10.0f);
    ImGui::SliderFloat("brightness", &m_posteffect->data.brightness, -1.0f, +1.0f);
    ImGui::SliderFloat("contrast", &m_posteffect->data.contrast, -1.0f, +1.0f);
    ImGui::SliderFloat("hue", &m_posteffect->data.hue, -1.0f, +1.0f);
    ImGui::SliderFloat("saturation", &m_posteffect->data.saturation, -1.0f, +1.0f);
    ImGui::SliderFloat("bloomextractionthreshold", &m_posteffect->data.bloomextractionthreshold, +0.0f, +10.0f);
    ImGui::SliderFloat("blurconvolutionintensity", &m_posteffect->data.blurconvolutionintensity, +0.0f, +10.0f);
  }

  //ライトのimgui
  LightManager::Instance().DrawDebugGUI();

  m_gBuffer->DrawImGui();

  ImGui::Text("OffScreen");
  ImGui::Image(m_offScreenBuffer[static_cast<size_t>(offscreen::offscreen)]->m_shaderresourceviews[0].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

  ImGui::Text("FinalPass");
  ImGui::Image(m_offScreenBuffer[static_cast<size_t>(offscreen::posteffect)]->m_shaderresourceviews[0].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

  ImGui::End();
}

void PostEffect::StartOffScreenRendering()
{
  ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

  m_offScreenBuffer[static_cast<int>(offscreen::offscreen)]->Clear(dc);
  m_offScreenBuffer[static_cast<int>(offscreen::offscreen)]->Activate(dc);
}
