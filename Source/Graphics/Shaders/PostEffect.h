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
private:

    PostEffect();
    ~PostEffect() {}

    // コピーコンストラクタと代入演算子を削除
    PostEffect(const PostEffect&) = delete;
    PostEffect& operator=(const PostEffect&) = delete;

public:

    // パラメータの種類を識別するための列挙型
    enum class PostEffectParameter
    {
        VignetteColor,
        Brightness,
        Contrast,
        Hue,
        Saturation,
        BloomExtractionThreshold,
        BlurConvolutionIntensity,
        Exposure,
        VignetteSize,
        VignetteIntensity,
        DistanceToSun,
        BlurStrength,
        BlurRadius,
        BlurDecay
    };

public:

    //シングルトン
    static PostEffect& Instance()
    {
        static PostEffect instance;
        return instance;
    }

    // デファードのレンダーターゲットを設定
    void SetDeferredTarget();

    // デファードのリソース設定
    void EndDeferred();

    // ポストエフェクト描画
    void PostEffectRender();

    // imgui描画
    void PostEffectImGui();

    // オフスクリーンバッファに描画していく
    void StartOffScreenRendering();

    // 深度マップをSRVにコピーして、GPUにバインドする
    void DepthCopyAndBind(int registerIndex);

    //画面サイズ変更時にレンダーターゲットを作り直す
    void ResizeBuffer();

    //シーンのimgui
    void SceneImGui();

    //ポストエフェクトのパラメータを制御する関数
    void ParameterMove(float elapsedTime, float parameterIn, bool update, PostEffectParameter parameter);

public:

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
        float distance_to_sun = 17.301f;
        float blurstrength = {};
        float blurradius = { 1.0f };
        float blurdecay = { 0.999f };
    };
    std::unique_ptr<ConstantBuffer<POSTEFFECT>>m_posteffect;

private:

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
};
