#pragma once

#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "ConstantBuffer.h"
#include "Bloom.h"
#include "MultiRenderTarget.h"
#include <DirectXMath.h>

//ポストエフェクト
class PostEffect
{
public:
    PostEffect();
    ~PostEffect() {}

    //デファードの最初の処理
    void DeferredFirstSet();

    //デファードのリソース設定
    void DeferredResourceSet();

    //ポストエフェクト描画
    void PostEffectRender();

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