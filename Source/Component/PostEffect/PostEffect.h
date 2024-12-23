#pragma once

#include "Graphics/PostEffect/FrameBuffer.h"
#include "Graphics/PostEffect/FullScreenQuad.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/PostEffect/Bloom.h"
#include "Graphics/PostEffect/MultiRenderTarget.h"
#include "Graphics/PostEffect/CascadedShadowMap.h"
#include <DirectXMath.h>
#include "Component\System\Component.h"
#include <map>

//�|�X�g�G�t�F�N�g
class PostEffect :public Component
{
public:

    PostEffect();
    ~PostEffect() {};

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName()const override { return "PostEffect"; }

public:

    // �p�����[�^�̎�ނ����ʂ��邽�߂̗񋓌^
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

    // �f�t�@�[�h�̃����_�[�^�[�Q�b�g��ݒ�
    void SetDeferredTarget();

    // �f�t�@�[�h�̃��\�[�X�ݒ�
    void EndDeferred();

    // �|�X�g�G�t�F�N�g�`��
    void PostEffectRender();

    // �I�t�X�N���[���o�b�t�@�ɕ`�悵�Ă���
    void StartOffScreenRendering();

    // �[�x�}�b�v��SRV�ɃR�s�[���āAGPU�Ƀo�C���h����
    void DepthCopyAndBind(int registerIndex);

    //�����Ńp�����[�^��enumclass��ݒ肷��
    void SetParameter(float endparameter, float timescale, const std::vector<PostEffectParameter>& parameters);

private:

    //�|�X�g�G�t�F�N�g�̃p�����[�^
    void UpdatePostEffectParameter(float elapsedTime);

private:

    //�|�X�g�G�t�F�N�g�̃R���X�^���g�o�b�t�@
    struct POSTEFFECT
    {
        DirectX::XMFLOAT4 vignettecolor = { 0.991f, 0.073f, 0.073f, 1.000f };
        float colorize[3] = { 1, 1, 1 };
        float brightness = 0.0f;
        float contrast = 0.10f;
        float hue = 0.000f;
        float saturation = 0.0f;
        float bloomextractionthreshold = 0.800f;
        float blurconvolutionintensity = 0.130f;
        float exposure = 1.4f;
        float vignettesize = 0.7f;
        float vignetteintensity = 0.01f;
        DirectX::XMFLOAT4 ssrparameter = { 50.0f,10.0f,0.1f,1.0f };
        float blurstrength = {};
        float blurradius = { 1.0f };
        float blurdecay = { 0.999f };
        float padding = {};
    };
    std::unique_ptr<ConstantBuffer<POSTEFFECT>>m_posteffect;

    //�e�̃p�����[�^�̃R���X�^���g�o�b�t�@
    struct SHADOWPARAMETER
    {
        float shadowdepthbias = 0.00008475f;
        float shadowcolor = 0.419f;
        float shadowfilterradius = 14.222f;
        int shadowsamplecount = 32;
    };
    std::unique_ptr<ConstantBuffer<SHADOWPARAMETER>>m_shadowparameter;

public:

    //�擾
    CascadedShadowMap* GetCascadedShadow() const { return m_cascadedshadowmap.get(); }
    MultiRenderTarget* GetMultiRenderTarget() const { return m_gBuffer.get(); }
    float m_criticaldepthvalue = 300.0f;

    const POSTEFFECT& GetPostData() { return m_posteffect->data; }
    void SetExposureZero() { m_posteffect->data.exposure = 0; }

private:
    enum class offscreen { offscreen, posteffect, tonemap, cascadeshadow, fxaa, depthCopy, max };
    enum class pixelshader { deferred, colorGrading, cascadeshadow, fxaa, tonemap, max };
    std::unique_ptr<FrameBuffer> m_offScreenBuffer[static_cast<int>(offscreen::max)];
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelshaders[static_cast<int>(pixelshader::max)];

    std::unique_ptr<Bloom> m_bloomeffect;
    std::unique_ptr<MultiRenderTarget>m_gBuffer;
    std::unique_ptr<CascadedShadowMap> m_cascadedshadowmap;

    // �����o�ϐ�
    struct ParamState
    {
        float targetValue = 0.0f;
        float timeScale = 0.0f; // �p�����[�^�̕ω����x
    };
    std::map<PostEffect::PostEffectParameter, ParamState> paramStates;
};