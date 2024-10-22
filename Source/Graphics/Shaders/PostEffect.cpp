#include "PostEffect.h"
#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include "Graphics/Shaders/Texture.h"
#include <imgui.h>
#include "Shader.h"

//�R���X�g���N�^
PostEffect::PostEffect()
{
    Graphics& Graphics = Graphics::Instance();

    //�u���[���Z�b�g
    m_bloomeffect = std::make_unique<Bloom>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight());

    //�t���[���o�b�t�@����
    for (int i = 0; i < static_cast<int>(offscreen::max); ++i) {
        m_offScreenBuffer[i] = std::make_unique<FrameBuffer>(Graphics.GetDevice(),
            static_cast<uint32_t>(Graphics.GetScreenWidth()), static_cast<uint32_t>(Graphics.GetScreenHeight()),
            DXGI_FORMAT_R32G32B32A32_FLOAT, true);
    }

    //�s�N�Z���V�F�[�_�[
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\DeferredPBR_PS.cso", m_pixelshaders[static_cast<int>(pixelshader::deferred)].GetAddressOf());
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\ColorGradingPS.cso", m_pixelshaders[static_cast<int>(pixelshader::colorGrading)].GetAddressOf());
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\CascadeShadow.cso", m_pixelshaders[static_cast<int>(pixelshader::cascadeshadow)].GetAddressOf());
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\SSR.cso", m_pixelshaders[static_cast<int>(pixelshader::ssr)].GetAddressOf());
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\ToneMapPS.cso", m_pixelshaders[static_cast<int>(pixelshader::tonemap)].GetAddressOf());
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\FXAA.cso", m_pixelshaders[static_cast<int>(pixelshader::fxaa)].GetAddressOf());
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\DecalPS.cso", m_pixelshaders[static_cast<int>(pixelshader::decal)].GetAddressOf());

    //MultiRenderTarget�쐬
    m_gBuffer = std::make_unique<decltype(m_gBuffer)::element_type>(Graphics.GetDevice(), Graphics.GetScreenWidth(), Graphics.GetScreenHeight(), 6);

    //�e�쐬
    m_cascadedshadowmap = std::make_unique<CascadedShadowMap>(Graphics.GetDevice(), 1024 * 4, 1024 * 4);

    //�R���X�^���g�o�b�t�@
    m_posteffect = std::make_unique<ConstantBuffer<POSTEFFECT>>(Graphics.GetDevice());
    m_shadowparameter = std::make_unique<ConstantBuffer<SHADOWPARAMETER>>(Graphics.GetDevice());
}

//�f�t�@�[�h�̍ŏ��̏���
void PostEffect::SetDeferredTarget()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //skymap�`��
    SkyBoxManager::Instance().DrawSkyBox(dc);

    //MultiRenderTarget�̕`��i�K
    m_gBuffer->clear(dc);
    m_gBuffer->activate(dc);
}

//�f�t�@�[�h�̏I��
void PostEffect::EndDeferred()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    // G�o�b�t�@�p�̃����_�[�^�[�Q�b�g��GPU������
    m_gBuffer->LiberationRenderTarget(dc);

    // �X�J�C�{�b�N�X�̃��\�[�X���o�C���h
    SkyBoxManager::Instance().BindTextures(dc, 10);

    // �[�x�l�̏������݂͂��Ȃ�
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);

    // G�o�b�t�@�����ɕ`�� ( PBR + IBL )
    ID3D11ShaderResourceView* d[]
    { m_gBuffer->GetShaderResources()[0],m_gBuffer->GetShaderResources()[1],m_gBuffer->GetShaderResources()[2],m_gBuffer->GetShaderResources()[3] ,m_gBuffer->GetShaderResources()[4] };
    FullScreenQuad::Instance().Blit(dc, m_gBuffer->GetShaderResources(), 0, _countof(d), m_pixelshaders[static_cast<int>(pixelshader::deferred)].Get());

    // �[�x�X�e���V����߂�
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 1);
}

//�|�X�g�G�t�F�N�g�`��
void PostEffect::PostEffectRender()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // G�o�b�t�@��DSV��GPU������
    m_gBuffer->LiberationDepthStencil(dc);
    m_gBuffer->ReleaseCache(dc);
    // �I�t�X�N���[���ւ̕`����~�߂�
    m_offScreenBuffer[static_cast<int>(offscreen::offscreen)]->Deactivate(dc);

    //�R���X�^���g�o�b�t�@�̃A�N�e�B�u
    m_posteffect->Activate(dc, (int)CB_INDEX::POST_EFFECT, true, true, false, false, false, false);
    m_shadowparameter->Activate(dc, (int)CB_INDEX::SHADOW_PAR, false, true, false, false, false, false);

    //SSR
    m_offScreenBuffer[static_cast<int>(offscreen::ssr)]->Clear(dc);
    m_offScreenBuffer[static_cast<int>(offscreen::ssr)]->Activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::ADD), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* ssr[]
    { m_offScreenBuffer[static_cast<size_t>(offscreen::offscreen)]->m_shaderresourceviews[0].Get() ,*m_gBuffer->GetDepthStencilSRV(),m_gBuffer->GetShaderResources()[1],m_gBuffer->GetShaderResources()[4] };
    FullScreenQuad::Instance().Blit(dc, ssr, 0, _countof(ssr), m_pixelshaders[static_cast<int>(pixelshader::ssr)].Get());
    m_offScreenBuffer[static_cast<int>(offscreen::ssr)]->Deactivate(dc);

    //�e
    m_offScreenBuffer[static_cast<int>(offscreen::cascadeshadow)]->Clear(dc);
    m_offScreenBuffer[static_cast<int>(offscreen::cascadeshadow)]->Activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::ALPHA), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* shadow[]
    { m_offScreenBuffer[static_cast<size_t>(offscreen::ssr)]->m_shaderresourceviews[0].Get() ,*m_gBuffer->GetDepthStencilSRV(),m_cascadedshadowmap->m_shaderresourceview.Get() };
    FullScreenQuad::Instance().Blit(dc, shadow, 0, _countof(shadow), m_pixelshaders[static_cast<int>(pixelshader::cascadeshadow)].Get());
    m_offScreenBuffer[static_cast<int>(offscreen::cascadeshadow)]->Deactivate(dc);

    //�f�J�[��
    //m_offScreenBuffer[static_cast<int>(offscreen::decal)]->Clear(dc);
    //m_offScreenBuffer[static_cast<int>(offscreen::decal)]->Activate(dc);
    //dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::ALPHA), nullptr, 0xFFFFFFFF);
    //dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    //dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    //ID3D11ShaderResourceView* decal[]
    //{ m_offScreenBuffer[static_cast<size_t>(offscreen::ssr)]->m_shaderresourceviews[0].Get() ,m_gBuffer->GetShaderResources()[2] };
    //FullScreenQuad::Instance().Blit(dc, decal, 0, _countof(decal), m_pixelshaders[static_cast<int>(pixelshader::decal)].Get());
    //m_offScreenBuffer[static_cast<int>(offscreen::decal)]->Deactivate(dc);

    //�|�X�g�G�t�F�N�g
    m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->Clear(dc);
    m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->Activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* posteffect[]
    { m_offScreenBuffer[static_cast<size_t>(offscreen::cascadeshadow)]->m_shaderresourceviews[0].Get(),*m_gBuffer->GetDepthStencilSRV() ,m_gBuffer->GetShaderResources()[5] };
    FullScreenQuad::Instance().Blit(dc, posteffect, 0, _countof(posteffect), m_pixelshaders[static_cast<int>(pixelshader::colorGrading)].Get());
    m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->Deactivate(dc);

    //�u���[��
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

    //FXAA
    m_offScreenBuffer[static_cast<int>(offscreen::fxaa)]->Clear(dc);
    m_offScreenBuffer[static_cast<int>(offscreen::fxaa)]->Activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* fxaa[] = { m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->m_shaderresourceviews[0].Get() };
    FullScreenQuad::Instance().Blit(dc, fxaa, 0, _countof(fxaa), m_pixelshaders[static_cast<int>(pixelshader::fxaa)].Get());
    m_offScreenBuffer[static_cast<int>(offscreen::posteffect)]->Deactivate(dc);

    // �g�[���}�b�v����
    //m_offScreenBuffer[static_cast<int>(offscreen::tonemap)]->Clear(dc);
    //m_offScreenBuffer[static_cast<int>(offscreen::tonemap)]->Activate(dc);
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* tone[] = { m_offScreenBuffer[static_cast<int>(offscreen::fxaa)]->m_shaderresourceviews[0].Get() };
    FullScreenQuad::Instance().Blit(dc, tone, 0, _countof(tone), m_pixelshaders[static_cast<int>(pixelshader::tonemap)].Get());
    //m_offScreenBuffer[static_cast<int>(offscreen::fxaa)]->Deactivate(dc);
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
        ImGui::ColorEdit4("vignettecolor", &m_posteffect->data.vignettecolor.x);
        ImGui::DragFloat("vignettesize", &m_posteffect->data.vignettesize, 0.1f, 0.0f, 2.0f);
        ImGui::DragFloat("vignetteintensity", &m_posteffect->data.vignetteintensity, 0.1f, 0.0f, 2.0f);
        ImGui::SliderFloat("distance_to_sun", &m_posteffect->data.distance_to_sun, 0.0f, 1000.0f);
        ImGui::DragFloat4("ssrparameter", &m_posteffect->data.ssrparameter.x, 0.1f);
        ImGui::SliderFloat("blurstrength", &m_posteffect->data.blurstrength, +0.0f, +1.0f);
        ImGui::SliderFloat("blurradius", &m_posteffect->data.blurradius, +0.0f, +1.0f);
        ImGui::SliderFloat("blurdecay", &m_posteffect->data.blurdecay, +0.0f, +1.0f);
    }

    //���C�g��imgui
    LightManager::Instance().DrawDebugGUI();

    if (ImGui::TreeNode("shadow"))
    {
        ImGui::DragFloat("split_scheme_weight", &m_cascadedshadowmap->m_splitschemeweight, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("critical_depth_value", &m_criticaldepthvalue, 1.0f, 0.0f, 1000.0f);
        ImGui::SliderFloat("shadow_color", &m_shadowparameter->data.shadowcolor, 0.0f, 1.0f);
        ImGui::SliderFloat("shadow_depth_bias", &m_shadowparameter->data.shadowdepthbias, 0.0f, 0.005f, "%.8f");
        ImGui::SliderFloat("shadow_filter_radius", &m_shadowparameter->data.shadowfilterradius, 0.0f, 64.0f);
        ImGui::SliderInt("shadow_sample_count", reinterpret_cast<int*>(&m_shadowparameter->data.shadowsamplecount), 0, 64);
        ImGui::Image(m_cascadedshadowmap->m_shaderresourceview.Get(), { 256, 256 });
        ImGui::TreePop();
    }

    m_gBuffer->DrawImGui();

    ImGui::Text("OffScreen");
    ImGui::Image(m_offScreenBuffer[static_cast<size_t>(offscreen::offscreen)]->m_shaderresourceviews[0].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

    ImGui::Text("FinalPass");
    ImGui::Image(m_offScreenBuffer[static_cast<size_t>(offscreen::tonemap)]->m_shaderresourceviews[0].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

    ImGui::End();
}

void PostEffect::StartOffScreenRendering()
{
    ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

    m_offScreenBuffer[static_cast<int>(offscreen::offscreen)]->Clear(dc);
    m_offScreenBuffer[static_cast<int>(offscreen::offscreen)]->Activate(dc);
}

void PostEffect::DepthCopyAndBind(int registerIndex)
{
    ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

    m_offScreenBuffer[static_cast<int>(offscreen::depthCopy)]->Clear(dc);
    m_offScreenBuffer[static_cast<int>(offscreen::depthCopy)]->Activate(dc);

    FullScreenQuad::Instance().Blit(dc, m_gBuffer->GetDepthStencilSRV(), 0, 1);

    m_offScreenBuffer[static_cast<int>(offscreen::depthCopy)]->Deactivate(dc);

    dc->PSSetShaderResources(registerIndex, 1,
        m_offScreenBuffer[static_cast<int>(offscreen::depthCopy)]->m_shaderresourceviews[0].GetAddressOf());
}

//��ʃT�C�Y�ύX���Ƀ����_�[�^�[�Q�b�g����蒼��
void PostEffect::ResizeBuffer()
{
    Graphics& Graphics = Graphics::Instance();
    const UINT& width = Graphics.GetScreenWidth();
    const UINT& height = Graphics.GetScreenHeight();

    //�u���[���Z�b�g
    m_bloomeffect = std::make_unique<Bloom>(Graphics.GetDevice(), width, height);

    //�t���[���o�b�t�@����
    for (int i = 0; i < static_cast<int>(offscreen::max); ++i)
    {
        m_offScreenBuffer[i] = std::make_unique<FrameBuffer>(Graphics.GetDevice(), width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, true);
    }

    //MultiRenderTarget�쐬
    m_gBuffer = std::make_unique<decltype(m_gBuffer)::element_type>(Graphics.GetDevice(), width, height, 6);
}

//�V�[����imgui
void PostEffect::SceneImGui()
{
    //ImGuiIO& io = ImGui::GetIO();

    //// ImGui��Ƀ}�E�X�J�[�\��������ꍇ�͏������Ȃ�
    //if (io.WantCaptureMouse) return;

    ImGui::Begin("GameScene");
    // �E�B���h�E�̈ʒu�ƃT�C�Y���擾
    ImVec2 size = ImGui::GetContentRegionAvail();
    // �t���[���o�b�t�@�̃V�F�[�_�[���\�[�X�r���[���擾
    ImTextureID texture_id = (ImTextureID)(m_offScreenBuffer[static_cast<size_t>(offscreen::tonemap)]->m_shaderresourceviews[0].Get());
    // �摜��\��
    ImGui::Image(texture_id, size);
    ImGui::End();
}

//�|�X�g�G�t�F�N�g�̃p�����[�^�𐧌䂷��֐�
void PostEffect::ParameterMove(float elapsedTime, float parameterIn, bool update, PostEffectParameter parameter)
{
    if (!m_posteffect) return;  // m_posteffect�������ȏꍇ�͏������Ȃ�
    POSTEFFECT& data = m_posteffect->data;

    if (update)
    {
        // �X�V���K�v�ȏꍇ
        switch (parameter)
        {
        case PostEffectParameter::Brightness:
            data.brightness = parameterIn;
            break;
        case PostEffectParameter::Contrast:
            data.contrast = parameterIn;
            break;
        case PostEffectParameter::Hue:
            data.hue = parameterIn;
            break;
        case PostEffectParameter::Saturation:
            data.saturation = parameterIn;
            break;
        case PostEffectParameter::BloomExtractionThreshold:
            data.bloomextractionthreshold = parameterIn;
            break;
        case PostEffectParameter::BlurConvolutionIntensity:
            data.blurconvolutionintensity = parameterIn;
            break;
        case PostEffectParameter::Exposure:
            data.exposure = parameterIn;
            break;
        case PostEffectParameter::VignetteSize:
            data.vignettesize = parameterIn;
            break;
        case PostEffectParameter::VignetteIntensity:
            data.vignetteintensity = parameterIn;
            break;
        case PostEffectParameter::DistanceToSun:
            data.distance_to_sun = parameterIn;
            break;
        case PostEffectParameter::BlurStrength:
            data.blurstrength = parameterIn;
            break;
        case PostEffectParameter::BlurRadius:
            data.blurradius = parameterIn;
            break;
        case PostEffectParameter::BlurDecay:
            data.blurdecay = parameterIn;
            break;
        default:
            break;
        }
    }
    else
    {
        // ���Ԃɉ����Č���������ꍇ
        switch (parameter)
        {
        case PostEffectParameter::Brightness:
            data.brightness = (std::max)(0.0f, data.brightness - elapsedTime);
            break;
        case PostEffectParameter::Contrast:
            data.contrast = (std::max)(0.0f, data.contrast - elapsedTime);
            break;
        case PostEffectParameter::Hue:
            data.hue = (std::max)(0.0f, data.hue - elapsedTime);
            break;
        case PostEffectParameter::Saturation:
            data.saturation = (std::max)(0.0f, data.saturation - elapsedTime);
            break;
        case PostEffectParameter::BloomExtractionThreshold:
            data.bloomextractionthreshold = (std::max)(0.0f, data.bloomextractionthreshold - elapsedTime);
            break;
        case PostEffectParameter::BlurConvolutionIntensity:
            data.blurconvolutionintensity = (std::max)(0.0f, data.blurconvolutionintensity - elapsedTime);
            break;
        case PostEffectParameter::Exposure:
            data.exposure = (std::max)(0.0f, data.exposure - elapsedTime);
            break;
        case PostEffectParameter::VignetteSize:
            data.vignettesize = (std::max)(0.0f, data.vignettesize - elapsedTime);
            break;
        case PostEffectParameter::VignetteIntensity:
            data.vignetteintensity = (std::max)(0.01f, data.vignetteintensity - elapsedTime);
            break;
        case PostEffectParameter::DistanceToSun:
            data.distance_to_sun = (std::max)(0.0f, data.distance_to_sun - elapsedTime);
            break;
        case PostEffectParameter::BlurStrength:
            data.blurstrength = (std::max)(0.0f, data.blurstrength - elapsedTime);
            break;
        case PostEffectParameter::BlurRadius:
            data.blurradius = (std::max)(0.0f, data.blurradius - elapsedTime);
            break;
        case PostEffectParameter::BlurDecay:
            data.blurdecay = (std::max)(0.0f, data.blurdecay - elapsedTime);
            break;
        default:
            break;
        }
    }
}