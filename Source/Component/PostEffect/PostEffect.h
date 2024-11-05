#pragma once

#include "Graphics/PostEffect/FrameBuffer.h"
#include "Graphics/PostEffect/FullScreenQuad.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/PostEffect/Bloom.h"
#include "Graphics/PostEffect/MultiRenderTarget.h"
#include "Graphics/PostEffect/CascadedShadowMap.h"
#include <DirectXMath.h>
#include "Component\System\Component.h"

//ポストエフェクト
class PostEffect :public Component
{
public:

    PostEffect();
    ~PostEffect() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName()const override { return "PostEffect"; }

public:

    // パラメータの種類を識別するための列挙型
    enum class PostEffectParameter
    {
        Brightness,
        Contrast,
        Hue,
        Saturation,
        Exposure,
        VignetteSize,
        VignetteIntensity,
        BlurStrength,
        BlurRadius,
        BlurDecay,
        MAX
    };

    // デファードのレンダーターゲットを設定
    void SetDeferredTarget();

    // デファードのリソース設定
    void EndDeferred();

    // ポストエフェクト描画
    void PostEffectRender();

    // オフスクリーンバッファに描画していく
    void StartOffScreenRendering();

    // 深度マップをSRVにコピーして、GPUにバインドする
    void DepthCopyAndBind(int registerIndex);

    //ここでパラメータとenumclassを設定する
    void SetParameter(float endparameter, float timescale, PostEffectParameter PP);

private:

    //ポストエフェクトのパラメータ
    void UpdatePostEffectParameter(float elapsedTime);

private:

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
        float exposure = 1.4f;
        float vignettesize = 0.8f;
        float vignetteintensity = 0.05f;
        DirectX::XMFLOAT4 ssrparameter = { 50.0f,10.0f,0.1f,1.0f };
        float distance_to_sun = 100.361f;
        float blurstrength = {};
        float blurradius = { 1.0f };
        float blurdecay = { 0.999f };
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

    //取得
    CascadedShadowMap* GetCascadedShadow() const { return m_cascadedshadowmap.get(); }
    MultiRenderTarget* GetMultiRenderTarget() const { return m_gBuffer.get(); }
    float m_criticaldepthvalue = 300.0f;

private:
    enum class offscreen { offscreen, posteffect, tonemap, cascadeshadow, ssr, fxaa, depthCopy, max };
    enum class pixelshader { deferred, colorGrading, cascadeshadow, ssr, fxaa, tonemap, max };
    std::unique_ptr<FrameBuffer> m_offScreenBuffer[static_cast<int>(offscreen::max)];
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelshaders[static_cast<int>(pixelshader::max)];

    std::unique_ptr<Bloom> m_bloomeffect;
    std::unique_ptr<MultiRenderTarget>m_gBuffer;
    std::unique_ptr<CascadedShadowMap> m_cascadedshadowmap;

    //ここパラメータ
    float originalparameter = 0.0f;
    float timescale = {};
    PostEffectParameter p = PostEffectParameter::Contrast;
};