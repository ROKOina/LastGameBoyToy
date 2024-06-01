#include "PostEffect.h"
#include "Graphics.h"
#include "Light/LightManager.h"
#include <imgui.h>

//コンストラクタ
PostEffect::PostEffect()
{
    Graphics& Graphics = Graphics::Instance();

    //ブルームセット
    bloom_effect = std::make_unique<Bloom>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight());

    //フレームバッファ生成
    framebuffers[static_cast<size_t>(offscreen::posteffect)] = std::make_unique<FrameBuffer>(Graphics.GetDevice(), static_cast<uint32_t>(Graphics.GetScreenWidth()), static_cast<uint32_t>(Graphics.GetScreenHeight()), DXGI_FORMAT_R32G32B32A32_FLOAT, false);
    framebuffers[static_cast<size_t>(offscreen::offscreen)] = std::make_unique<FrameBuffer>(Graphics.GetDevice(), static_cast<uint32_t>(Graphics.GetScreenWidth()), static_cast<uint32_t>(Graphics.GetScreenHeight()), DXGI_FORMAT_R32G32B32A32_FLOAT, true);

    //ピクセルシェーダー
    create_ps_from_cso(Graphics.GetDevice(), "Shader\\PostEffectPS.cso", pixelshaders[static_cast<size_t>(pixelshader::posteffect)].GetAddressOf());
    create_ps_from_cso(Graphics.GetDevice(), "Shader\\ToneMapPS.cso", pixelshaders[static_cast<size_t>(pixelshader::tonemap)].GetAddressOf());

    //フルスクリーンクアッド生成
    bit_block_transfer = std::make_unique<FullScreenQuad>(Graphics.GetDevice());

    //ポストエフェクトのコンスタントバッファ
    posteffect = std::make_unique<constant_buffer<POSTEFFECT>>(Graphics.GetDevice());
}

//モデルを描画する前にこの関数セット
void PostEffect::BeforeSet()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //サンプラーステートの設定
    Graphics.SetSamplerState();

    //オフスクリーンのレンダーターゲットのクリアと現在のレンダーターゲットと深度ステンシルの状態をキャッシュ
    framebuffers[static_cast<size_t>(offscreen::offscreen)]->clear(dc);
    framebuffers[static_cast<size_t>(offscreen::offscreen)]->activate(dc);

    //skymap描画
    bit_block_transfer->SkyMap();
}

//モデルを描画後にこの関数をセットする
void PostEffect::AfterSet()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //コンスタントバッファのアクティブ
    posteffect->activate(dc, 3, true, true, false, false, false, false);

    //オフスクリーンレンダリングの情報を保持しておく
    framebuffers[static_cast<size_t>(offscreen::offscreen)]->deactivate(dc);

    //ポストエフェクト
    framebuffers[static_cast<size_t>(offscreen::posteffect)]->clear(dc);
    framebuffers[static_cast<size_t>(offscreen::posteffect)]->activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_OFF), 0);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    //リソース設定
    ID3D11ShaderResourceView* posteffect[]
    {
        framebuffers[static_cast<size_t>(offscreen::offscreen)]->shader_resource_views[0].Get(),
    };
    bit_block_transfer->blit(dc, posteffect, 0, _countof(posteffect), pixelshaders[static_cast<size_t>(pixelshader::posteffect)].Get());

    //一個前のポストエフェクトの情報を保存しておく
    framebuffers[static_cast<size_t>(offscreen::posteffect)]->deactivate(dc);

    //ブルーム
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 0);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    bloom_effect->make(dc, framebuffers[static_cast<size_t>(offscreen::posteffect)]->shader_resource_views[0].Get());
    framebuffers[static_cast<size_t>(offscreen::posteffect)]->activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::ADD), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 0);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    bloom_effect->blit(dc);
    framebuffers[static_cast<size_t>(offscreen::posteffect)]->deactivate(dc);

    //トーンマップ
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 0);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* tone[]{ framebuffers[static_cast<size_t>(offscreen::posteffect)]->shader_resource_views[0].Get() };
    bit_block_transfer->blit(dc, tone, 0, 1, pixelshaders[static_cast<size_t>(pixelshader::tonemap)].Get());
}

//imgui描画
void PostEffect::PostEffectImGui()
{
    ImGui::Begin("PostEffect");
    //ポストエフェクト
    if (ImGui::CollapsingHeader("PostEffect", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::ColorEdit3("colorize", posteffect->data.colorize);
        ImGui::SliderFloat("exposure", &posteffect->data.exposure, +0.0f, +10.0f);
        ImGui::SliderFloat("brightness", &posteffect->data.brightness, -1.0f, +1.0f);
        ImGui::SliderFloat("contrast", &posteffect->data.contrast, -1.0f, +1.0f);
        ImGui::SliderFloat("hue", &posteffect->data.hue, -1.0f, +1.0f);
        ImGui::SliderFloat("saturation", &posteffect->data.saturation, -1.0f, +1.0f);
        ImGui::SliderFloat("bloom_extraction_threshold", &posteffect->data.bloom_extraction_threshold, +0.0f, +10.0f);
        ImGui::SliderFloat("blur_convolution_intensity", &posteffect->data.blur_convolution_intensity, +0.0f, +10.0f);
    }
    //ライトのimgui
    LightManager::Instance().DrawDebugGUI();
    ImGui::End();
}