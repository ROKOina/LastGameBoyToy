#include "PostEffect.h"
#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include <imgui.h>
#include "Shader.h"

//コンストラクタ
PostEffect::PostEffect()
{
    Graphics& Graphics = Graphics::Instance();

    //ブルームセット
    m_bloomeffect = std::make_unique<Bloom>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight());

    //フレームバッファ生成
    m_framebuffers[static_cast<size_t>(offscreen::posteffect)] = std::make_unique<FrameBuffer>(Graphics.GetDevice(), static_cast<uint32_t>(Graphics.GetScreenWidth()), static_cast<uint32_t>(Graphics.GetScreenHeight()), DXGI_FORMAT_R32G32B32A32_FLOAT, false);

    //ピクセルシェーダー
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\PostEffectPS.cso", m_pixelshaders[static_cast<size_t>(pixelshader::posteffect)].GetAddressOf());
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\ToneMapPS.cso", m_pixelshaders[static_cast<size_t>(pixelshader::tonemap)].GetAddressOf());

    //フルスクリーンクアッド生成
    m_bitblocktransfer = std::make_unique<FullScreenQuad>(Graphics.GetDevice());

    //MultiRenderTarget作成
    m_multirendertarget = std::make_unique<decltype(m_multirendertarget)::element_type>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight(), 5);

    //コンスタントバッファ
    m_posteffect = std::make_unique<ConstantBuffer<POSTEFFECT>>(Graphics.GetDevice());
}

//デファードの最初の処理
void PostEffect::DeferredFirstSet()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //MultiRenderTargetの描画段階
    m_multirendertarget->clear(dc);
    m_multirendertarget->activate(dc);

    //skymap描画
    m_bitblocktransfer->SkyMap();
}

//デファードのリソース設定
void PostEffect::DeferredResourceSet()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    // MULTIPLE_RENDER_TARGETS
    m_multirendertarget->deactivate(dc);
}

//ポストエフェクト描画
void PostEffect::PostEffectRender()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //コンスタントバッファのアクティブ
    m_posteffect->Activate(dc, 3, true, true, false, false, false, false);

    //ポストエフェクト
    m_framebuffers[static_cast<size_t>(offscreen::posteffect)]->Clear(dc);
    m_framebuffers[static_cast<size_t>(offscreen::posteffect)]->Activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* posteffect[]
    {
       m_multirendertarget->m_rendertargetshaderresourceviews[0],
    };
    m_bitblocktransfer->Blit(dc, posteffect, 0, _countof(posteffect), m_pixelshaders[static_cast<size_t>(pixelshader::posteffect)].Get());
    m_framebuffers[static_cast<size_t>(offscreen::posteffect)]->Deactivate(dc);

    //ブルーム
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    m_bloomeffect->Make(dc, m_framebuffers[static_cast<size_t>(offscreen::posteffect)]->m_shaderresourceviews[0].Get());
    m_framebuffers[static_cast<size_t>(offscreen::posteffect)]->Activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::ADD), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    m_bloomeffect->Blit(dc);
    m_framebuffers[static_cast<size_t>(offscreen::posteffect)]->Deactivate(dc);

    //トーンマップ
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* tone[]{ m_framebuffers[static_cast<size_t>(offscreen::posteffect)]->m_shaderresourceviews[0].Get() };
    m_bitblocktransfer->Blit(dc, tone, 0, _countof(tone), m_pixelshaders[static_cast<size_t>(pixelshader::tonemap)].Get());
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

    //RenderTarget
    if (ImGui::TreeNode("rendertarget"))
    {
        ImGui::Text("Color");
        ImGui::Image(m_multirendertarget->m_rendertargetshaderresourceviews[0], { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        ImGui::Text("Normal");
        ImGui::Image(m_multirendertarget->m_rendertargetshaderresourceviews[1], { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        ImGui::Text("Position");
        ImGui::Image(m_multirendertarget->m_rendertargetshaderresourceviews[2], { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        ImGui::Text("Roughness,Metallic,AO");
        ImGui::Image(m_multirendertarget->m_rendertargetshaderresourceviews[3], { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        ImGui::Text("Emission");
        ImGui::Image(m_multirendertarget->m_rendertargetshaderresourceviews[4], { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        ImGui::Text("Effect");
        ImGui::Image(m_multirendertarget->m_rendertargetshaderresourceviews[5], { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        ImGui::Text("Depth");
        ImGui::Image(m_multirendertarget->m_depthstencilshaderresourceview, { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        ImGui::Text("FinalPass");
        ImGui::Image(m_framebuffers[static_cast<size_t>(offscreen::posteffect)]->m_shaderresourceviews[0].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        ImGui::TreePop();
    }
    ImGui::End();
}