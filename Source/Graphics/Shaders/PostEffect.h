#pragma once

#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "ConstantBuffer.h"
#include "Bloom.h"
#include "MultiRenderTarget.h"
#include "Graphics/Shaders/3D/CascadedShadowMap.h"
#include <DirectXMath.h>

//�|�X�g�G�t�F�N�g
class PostEffect
{
private:

    PostEffect();
    ~PostEffect() {}

    // �R�s�[�R���X�g���N�^�Ƒ�����Z�q���폜
    PostEffect(const PostEffect&) = delete;
    PostEffect& operator=(const PostEffect&) = delete;

public:

    // �p�����[�^�̎�ނ����ʂ��邽�߂̗񋓌^
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

    //�V���O���g��
    static PostEffect& Instance()
    {
        static PostEffect instance;
        return instance;
    }

    // �f�t�@�[�h�̃����_�[�^�[�Q�b�g��ݒ�
    void SetDeferredTarget();

    // �f�t�@�[�h�̃��\�[�X�ݒ�
    void EndDeferred();

    // �|�X�g�G�t�F�N�g�`��
    void PostEffectRender();

    // imgui�`��
    void PostEffectImGui();

    // �I�t�X�N���[���o�b�t�@�ɕ`�悵�Ă���
    void StartOffScreenRendering();

    // �[�x�}�b�v��SRV�ɃR�s�[���āAGPU�Ƀo�C���h����
    void DepthCopyAndBind(int registerIndex);

    //��ʃT�C�Y�ύX���Ƀ����_�[�^�[�Q�b�g����蒼��
    void ResizeBuffer();

    //�V�[����imgui
    void SceneImGui();

    //�|�X�g�G�t�F�N�g�̃p�����[�^�𐧌䂷��֐�
    void ParameterMove(float elapsedTime, float parameterIn, bool update, PostEffectParameter parameter);

public:

    //�|�X�g�G�t�F�N�g�̃R���X�^���g�o�b�t�@
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

private:
    enum class offscreen { offscreen, posteffect, tonemap, cascadeshadow, ssr, fxaa, depthCopy, max };
    enum class pixelshader { deferred, colorGrading, cascadeshadow, ssr, fxaa, tonemap, max };
    std::unique_ptr<FrameBuffer> m_offScreenBuffer[static_cast<int>(offscreen::max)];
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelshaders[static_cast<int>(pixelshader::max)];

    std::unique_ptr<Bloom> m_bloomeffect;
    std::unique_ptr<MultiRenderTarget>m_gBuffer;
    std::unique_ptr<CascadedShadowMap> m_cascadedshadowmap;
};
