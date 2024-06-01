#include "PostEffect.h"
#include "Graphics.h"
#include "Light/LightManager.h"
#include <imgui.h>

//�R���X�g���N�^
PostEffect::PostEffect()
{
    Graphics& Graphics = Graphics::Instance();

    //�u���[���Z�b�g
    bloom_effect = std::make_unique<Bloom>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight());

    //�t���[���o�b�t�@����
    framebuffers[static_cast<size_t>(offscreen::posteffect)] = std::make_unique<FrameBuffer>(Graphics.GetDevice(), static_cast<uint32_t>(Graphics.GetScreenWidth()), static_cast<uint32_t>(Graphics.GetScreenHeight()), DXGI_FORMAT_R32G32B32A32_FLOAT, false);
    framebuffers[static_cast<size_t>(offscreen::offscreen)] = std::make_unique<FrameBuffer>(Graphics.GetDevice(), static_cast<uint32_t>(Graphics.GetScreenWidth()), static_cast<uint32_t>(Graphics.GetScreenHeight()), DXGI_FORMAT_R32G32B32A32_FLOAT, true);

    //�s�N�Z���V�F�[�_�[
    create_ps_from_cso(Graphics.GetDevice(), "Shader\\PostEffectPS.cso", pixelshaders[static_cast<size_t>(pixelshader::posteffect)].GetAddressOf());
    create_ps_from_cso(Graphics.GetDevice(), "Shader\\ToneMapPS.cso", pixelshaders[static_cast<size_t>(pixelshader::tonemap)].GetAddressOf());

    //�t���X�N���[���N�A�b�h����
    bit_block_transfer = std::make_unique<FullScreenQuad>(Graphics.GetDevice());

    //�|�X�g�G�t�F�N�g�̃R���X�^���g�o�b�t�@
    posteffect = std::make_unique<constant_buffer<POSTEFFECT>>(Graphics.GetDevice());
}

//���f����`�悷��O�ɂ��̊֐��Z�b�g
void PostEffect::BeforeSet()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //�T���v���[�X�e�[�g�̐ݒ�
    Graphics.SetSamplerState();

    //�I�t�X�N���[���̃����_�[�^�[�Q�b�g�̃N���A�ƌ��݂̃����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V���̏�Ԃ��L���b�V��
    framebuffers[static_cast<size_t>(offscreen::offscreen)]->clear(dc);
    framebuffers[static_cast<size_t>(offscreen::offscreen)]->activate(dc);

    //skymap�`��
    bit_block_transfer->SkyMap();
}

//���f����`���ɂ��̊֐����Z�b�g����
void PostEffect::AfterSet()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //�R���X�^���g�o�b�t�@�̃A�N�e�B�u
    posteffect->activate(dc, 3, true, true, false, false, false, false);

    //�I�t�X�N���[�������_�����O�̏���ێ����Ă���
    framebuffers[static_cast<size_t>(offscreen::offscreen)]->deactivate(dc);

    //�|�X�g�G�t�F�N�g
    framebuffers[static_cast<size_t>(offscreen::posteffect)]->clear(dc);
    framebuffers[static_cast<size_t>(offscreen::posteffect)]->activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_OFF), 0);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    //���\�[�X�ݒ�
    ID3D11ShaderResourceView* posteffect[]
    {
        framebuffers[static_cast<size_t>(offscreen::offscreen)]->shader_resource_views[0].Get(),
    };
    bit_block_transfer->blit(dc, posteffect, 0, _countof(posteffect), pixelshaders[static_cast<size_t>(pixelshader::posteffect)].Get());

    //��O�̃|�X�g�G�t�F�N�g�̏���ۑ����Ă���
    framebuffers[static_cast<size_t>(offscreen::posteffect)]->deactivate(dc);

    //�u���[��
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

    //�g�[���}�b�v
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 0);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* tone[]{ framebuffers[static_cast<size_t>(offscreen::posteffect)]->shader_resource_views[0].Get() };
    bit_block_transfer->blit(dc, tone, 0, 1, pixelshaders[static_cast<size_t>(pixelshader::tonemap)].Get());
}

//imgui�`��
void PostEffect::PostEffectImGui()
{
    ImGui::Begin("PostEffect");
    //�|�X�g�G�t�F�N�g
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
    //���C�g��imgui
    LightManager::Instance().DrawDebugGUI();
    ImGui::End();
}