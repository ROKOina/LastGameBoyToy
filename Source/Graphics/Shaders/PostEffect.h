#pragma once

#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "ConstantBuffer.h"
#include "Bloom.h"
#include "MultiRenderTarget.h"
#include "Graphics/Shaders/3D/CascadedShadowMap.h"
#include <DirectXMath.h>

//ポストエフェクト
class PostEffect
{
public:
    PostEffect();
    ~PostEffect() {}

    // インスタンス取得
    static PostEffect& Instance() { return *instance_; }

    // デファードのレンダーターゲットを設定
    void SetDeferredTarget();

    // デファードのリソース設定
    void EndDeferred();

    // ポストエフェクト描画
    void PostEffectRender();

    // imgui描画
    void PostEffectImGui();

    //トーンマップ
    void ToneMapRender();

    // オフスクリーンバッファに描画していく
    void StartOffScreenRendering();

    // 深度マップをSRVにコピーして、GPUにバインドする
    void DepthCopyAndBind(int registerIndex);

    //行列更新
    void TransforUpdate();

public:

    //取得
    CascadedShadowMap* GetCascadedShadow() const { return m_cascadedshadowmap.get(); }
    MultiRenderTarget* GetMultiRenderTarget() const { return m_gBuffer.get(); }
    float m_criticaldepthvalue = 300.0f;

private:
    static PostEffect* instance_;

    //ポストエフェクトのコンスタントバッファ
    struct POSTEFFECT
    {
        DirectX::XMFLOAT4 vignettecolor = { 1.0f,0.0f,0.0f,1.0f };
        float colorize[3] = { 1, 1, 1 };
        float brightness = 0.0f;
        float contrast = 0.10f;
        float hue = 0.000f;
        float saturation = 0.0f;
        float bloomextractionthreshold = 0.800f;
        float blurconvolutionintensity = 0.200f;
        float exposure = 1.8f;
        float vignettesize = 0.8f;
        float vignetteintensity = 0.7f;
        DirectX::XMFLOAT4X4 DecalTransform = {};
    };
    std::unique_ptr<ConstantBuffer<POSTEFFECT>>m_posteffect;

    //影のパラメータのコンスタントバッファ
    struct SHADOWPARAMETER
    {
        float shadowdepthbias = 0.00008475f;
        float shadowcolor = 0.419f;
        float shadowfilterradius = 14.222f;
        int shadowsamplecount = 32;
    };
    std::unique_ptr<ConstantBuffer<SHADOWPARAMETER>>m_shadowparameter;

public:
    enum class offscreen { offscreen, posteffect, specialeffect, depthCopy, max };
    enum class pixelshader { deferred, colorGrading, tonemap, max };
    std::unique_ptr<FrameBuffer> m_offScreenBuffer[static_cast<int>(offscreen::max)];
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelshaders[static_cast<int>(pixelshader::max)];
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> decal;

    std::unique_ptr<Bloom> m_bloomeffect;
    std::unique_ptr<MultiRenderTarget>m_gBuffer;
    std::unique_ptr<CascadedShadowMap> m_cascadedshadowmap;

private:
    DirectX::XMFLOAT3 position = {};
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT3 rotation = {};
};