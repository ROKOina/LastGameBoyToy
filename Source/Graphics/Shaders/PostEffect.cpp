#include "PostEffect.h"
#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include <imgui.h>
#include "Shader.h"

//�R���X�g���N�^
PostEffect::PostEffect()
{
    Graphics& Graphics = Graphics::Instance();

    //�u���[���Z�b�g
    m_bloomeffect = std::make_unique<Bloom>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight());

    //�t���[���o�b�t�@����
    m_framebuffers[static_cast<size_t>(offscreen::posteffect)] = std::make_unique<FrameBuffer>(Graphics.GetDevice(), static_cast<uint32_t>(Graphics.GetScreenWidth()), static_cast<uint32_t>(Graphics.GetScreenHeight()), DXGI_FORMAT_R32G32B32A32_FLOAT, false);

    //�s�N�Z���V�F�[�_�[
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\PostEffectPS.cso", m_pixelshaders[static_cast<size_t>(pixelshader::posteffect)].GetAddressOf());
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\ToneMapPS.cso", m_pixelshaders[static_cast<size_t>(pixelshader::tonemap)].GetAddressOf());

    //�t���X�N���[���N�A�b�h����
    m_bitblocktransfer = std::make_unique<FullScreenQuad>(Graphics.GetDevice());

    //MultiRenderTarget�쐬
    m_multirendertarget = std::make_unique<decltype(m_multirendertarget)::element_type>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight(), 5);

    //�R���X�^���g�o�b�t�@
    m_posteffect = std::make_unique<ConstantBuffer<POSTEFFECT>>(Graphics.GetDevice());
}

//�f�t�@�[�h�̍ŏ��̏���
void PostEffect::DeferredFirstSet()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //MultiRenderTarget�̕`��i�K
    m_multirendertarget->clear(dc);
    m_multirendertarget->activate(dc);

    //skymap�`��
    m_bitblocktransfer->SkyMap();
}

//�f�t�@�[�h�̃��\�[�X�ݒ�
void PostEffect::DeferredResourceSet()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    // MULTIPLE_RENDER_TARGETS
    m_multirendertarget->deactivate(dc);
}

//�|�X�g�G�t�F�N�g�`��
void PostEffect::PostEffectRender()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //�R���X�^���g�o�b�t�@�̃A�N�e�B�u
    m_posteffect->Activate(dc, 3, true, true, false, false, false, false);

    //�|�X�g�G�t�F�N�g
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

    //�u���[��
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

    //�g�[���}�b�v
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* tone[]{ m_framebuffers[static_cast<size_t>(offscreen::posteffect)]->m_shaderresourceviews[0].Get() };
    m_bitblocktransfer->Blit(dc, tone, 0, _countof(tone), m_pixelshaders[static_cast<size_t>(pixelshader::tonemap)].Get());
}

//imgui�`��
void PostEffect::PostEffectImGui()
{
    ImGui::Begin("PostEffect");
    //�|�X�g�G�t�F�N�g
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

    //���C�g��imgui
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