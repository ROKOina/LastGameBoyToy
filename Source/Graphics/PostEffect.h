#pragma once

#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "constant_buffer.h"
#include "Bloom.h"
#include <DirectXMath.h>

//ポストエフェクト
class PostEffect
{
public:
    PostEffect();
    ~PostEffect() {}

    //モデルを描画する前にこの関数セット
    void BeforeSet();

    //モデルを描画後にこの関数をセットする
    void AfterSet();

    //imgui描画
    void PostEffectImGui();

private:

    //ポストエフェクトのコンスタントバッファ
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