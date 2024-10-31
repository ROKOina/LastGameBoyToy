#pragma once

#include "System\Component.h"
#include <wrl.h>
#include <d3d11.h>
#include "Graphics/Shaders/ConstantBuffer.h"

#define THREAD 1024

class GPUParticle :public Component
{
public:
    GPUParticle(const char* filename, size_t maxparticle);
    ~GPUParticle() {};

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override;

    //�`��
    void Render();

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName()const override { return "GPUParticle"; }

    // �G�t�F�N�g�Đ��֐�
    void Play();

    //���g�������֐�
    void DeleteMe(float elapsedTime);

private:
    //�V���A���C�Y
    void Serialize();

    //�f�V���A���C�Y
    void Desirialize(const char* filename);

    //�ǂݍ���
    void LoadDesirialize();

    //�p�����[�^���Z�b�g
    void ParameterReset();

    // �ݒ�Ȃǃp�����[�^�[�ȊO������GUI
    void SystemGUI();

    // �p�����[�^�[�֌W��GUI
    void ParameterGUI();

    void ColorGUI();
    void ScaleGUI();
    void SpeedGUI();
    void EmitGUI();

public:

    //�p�[�e�B�N���̎���
    struct EntityParticle
    {
        DirectX::XMFLOAT3 position = { 0,0,0 };
        DirectX::XMFLOAT2 scale = { 0.0f,0.0f };
        DirectX::XMFLOAT4 rotation = { 0,0,0,1 };
        DirectX::XMFLOAT3 velocity = { 0,0,0 };
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        float lifetime = 0.0f;
        float age = 0.0f;
        int isalive = {};
    };

    //�R���X�^���g�o�b�t�@
    struct GPUParticleConstants
    {
        DirectX::XMFLOAT4 rotation = { 0,0,0,1 };

        DirectX::XMFLOAT3 position = { 0,0,0 };
        int isalive = { true };

        DirectX::XMFLOAT3 currentEmitVec;
        int isEmitFlg = true;
    };
    std::unique_ptr<ConstantBuffer<GPUParticleConstants>>m_gpu;

    //�A�N�e�B�u��
    void SetLoop(const bool& loop) { m_GSC.isLoopFlg = loop; }

    //�ۑ�����R���X�^���g�o�b�t�@
    struct GPUparticleSaveConstants
    {
        float emitTime = 1.0f;
        float lifeTime = 1.0f;
        int stretchFlag = 0;
        int isLoopFlg = 0;

        DirectX::XMFLOAT4 shape = { 0,0.0f,0.0f,0.0f }; //���F�����ꏊ�����_���A���F���a�A���F���a�̃{�����[���A���F�~�`�ɂ���p��

        DirectX::XMFLOAT4 baseColor = { 1,1,1,1 };      // �x�[�X�ƂȂ�F

        DirectX::XMFLOAT4 lifeStartColor = { 1,1,1,1 }; // �p�[�e�B�N���̐������̐F
        DirectX::XMFLOAT4 lifeEndColor = { 1,1,1,1 };   // �p�[�e�B�N���̏��Ŏ��̐F
        // -------------------  ���� �ǂ��炩�̂�  ----------------------
        DirectX::XMFLOAT4 emitStartColor = { 1,1,1,1 }; // �G�t�F�N�g�̍Đ��J�n���̐F
        DirectX::XMFLOAT4 emitEndColor = { 1,1,1,1 };   // �G�t�F�N�g�Đ���̍ŏI�I�ȐF

        int colorVariateByLife = 0;               // �F�̕ω��̊���Ǘ� ( TRUE : �����ɂ���ĕω� )
        DirectX::XMFLOAT3 colorScale = { 1,1,1 }; // �F���X�ɖ��邭����ȂǂŎg�p

        DirectX::XMFLOAT3 emitVec = { 0,0,0 };
        float spiralSpeed = 0;

        DirectX::XMFLOAT3 orbitalVelocity = { 0,0,0 };
        float spiralstrong = 0;

        float veloRandScale = 0.0f;
        float speed = 0.0f;
        float emitStartSpeed = 1.0f;    //  �G�t�F�N�g�̍Đ��J�n���̑��x
        float emitEndSpeed = 1.0f;      //  �G�t�F�N�g�Đ���̍ŏI�I�ȑ��x

        DirectX::XMFLOAT2 scale = { 0.2f,0.2f };
        int scaleVariateByLife = 0;               // �傫���̕ω��̊���Ǘ� ( TRUE : �����ɂ���ĕω� )
        float padding6 = 0;               // �傫���̕ω��̊���Ǘ� ( TRUE : �����ɂ���ĕω� )

        float lifeStartSize = 1.0f;     // �p�[�e�B�N���̐������̑��x
        float lifeEndSize = 1.0f;       // �p�[�e�B�N���̏��Ŏ��̑��x
        // -------------------  ���� �ǂ��炩�̂�  ----------------------
        float emitStartSize = 1.0f;     // �G�t�F�N�g�̍Đ��J�n���̑��x
        float emitEndSize = 1.0f;       // �G�t�F�N�g�Đ���̍ŏI�I�ȑ��x

        float radial = { 0 };
        float buoyancy = {};
        float emitStartGravity = 0.0f;
        float emitEndGravity = 0.0f;

        float strechscale = { 1.0f };    //�X�g���b�`�r���{�[�h�̋��x(�L�т鎞�̑傫��)
        DirectX::XMFLOAT3 padding = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    GPUparticleSaveConstants m_GSC;

    //�ۑ�����p�����[�^
    struct SaveParameter
    {
        int m_blend = 2;
        int m_depthS = 2;
        std::string	m_textureName;

        //��������ʃo�[�W����
        bool m_deleteflag = false;
        float deletetime = 0.0f;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameter m_p;

private:
    float emitTimer = 0.0f;
    bool stopFlg = false;

    Microsoft::WRL::ComPtr<ID3D11Buffer>m_particlebuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>m_particlesrv;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>m_particleuav;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>m_geometryshader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>m_updatecomputeshader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>m_initialzecomputeshader;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>m_colormap;
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_constantbuffer;
    const size_t m_maxparticle = 0;
    float time = 0.0f;
};