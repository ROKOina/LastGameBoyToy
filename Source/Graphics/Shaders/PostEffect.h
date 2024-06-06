#pragma once

#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "ConstantBuffer.h"
#include "Bloom.h"
#include "MultiRenderTarget.h"
#include <DirectXMath.h>

//�|�X�g�G�t�F�N�g
class PostEffect
{
public:
    PostEffect();
    ~PostEffect() {}

    //�f�t�@�[�h�̍ŏ��̏���
    void DeferredFirstSet();

    //�f�t�@�[�h�̃��\�[�X�ݒ�
    void DeferredResourceSet();

    //�|�X�g�G�t�F�N�g�`��
    void PostEffectRender();

    //imgui�`��
    void PostEffectImGui();

private:

    //�|�X�g�G�t�F�N�g�̃R���X�^���g�o�b�t�@
    struct POSTEFFECT
    {
        float colorize[3] = { 1, 1, 1 };
        float brightness = 0.0f;
        float contrast = 0.10f;
        float hue = 0.000f;
        float saturation = 0.0f;
        float bloomextractionthreshold = 0.800f;
        float blurconvolutionintensity = 0.200f;
        float exposure = 1.8f;
        DirectX::XMFLOAT2 dummy = {};
    };
    std::unique_ptr<ConstantBuffer<POSTEFFECT>>m_posteffect;

private:
    enum class offscreen { posteffect, max };
    enum class pixelshader { posteffect, tonemap, max };
    std::unique_ptr<FrameBuffer> m_framebuffers[static_cast<int>(offscreen::max)];
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelshaders[static_cast<int>(pixelshader::max)];
    std::unique_ptr<FullScreenQuad> m_bitblocktransfer;
    std::unique_ptr<Bloom> m_bloomeffect;
    std::unique_ptr<MultiRenderTarget>m_multirendertarget;
};