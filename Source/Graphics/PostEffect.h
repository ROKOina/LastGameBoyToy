#pragma once

#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "constant_buffer.h"
#include "Bloom.h"
#include <DirectXMath.h>

//�|�X�g�G�t�F�N�g
class PostEffect
{
public:
    PostEffect();
    ~PostEffect() {}

    //���f����`�悷��O�ɂ��̊֐��Z�b�g
    void BeforeSet();

    //���f����`���ɂ��̊֐����Z�b�g����
    void AfterSet();

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
        float bloom_extraction_threshold = 0.800f;
        float blur_convolution_intensity = 0.200f;
        float exposure = 1.8f;
        DirectX::XMFLOAT2 dummy = {};
    };
    std::unique_ptr<constant_buffer<POSTEFFECT>>posteffect;

private:
    enum class offscreen { posteffect, offscreen };
    enum class pixelshader { posteffect, tonemap };
    std::unique_ptr<FrameBuffer> framebuffers[2];
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelshaders[2];
    std::unique_ptr<FullScreenQuad> bit_block_transfer;
    std::unique_ptr<Bloom> bloom_effect;
};