#pragma once

#include "System\Component.h"
#include <wrl.h>
#include <d3d11.h>
#include "Graphics/Shaders/ConstantBuffer.h"

#define THREAD 512

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

private:

    //���Z�b�g�֐�
    void Reset();

    //�V���A���C�Y
    void Serialize();

    //�f�V���A���C�Y
    void Desirialize(const char* filename);

    //�ǂݍ���
    void LoadDesirialize();

    //�p�����[�^���Z�b�g
    void ParameterReset();

public:

    //�p�[�e�B�N���̎���
    struct EntityParticle
    {
        DirectX::XMFLOAT3 position = { 0,0,0 };
        DirectX::XMFLOAT2 scale = { 0.0f,0.0f };
        DirectX::XMFLOAT4 rotation = { 0,0,0,1 };
        DirectX::XMFLOAT3 velocity = { 0,0,0 };
        DirectX::XMFLOAT3 strechvelocity = { 0,0,0 };
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        float lifetime = 0.0f;
        float age = 0.0f;
        int isalive = {};
        int isstart = 0;
    };

    //�R���X�^���g�o�b�t�@
    struct GPUParticleConstants
    {
        DirectX::XMFLOAT4 rotation = { 0,0,0,1 };
        DirectX::XMFLOAT3 position = { 0,0,0 };
        int isalive = { true };
        int loop = { true };
        int startflag = { true };
        DirectX::XMFLOAT2 Gdummy = {};
    };
    std::unique_ptr<ConstantBuffer<GPUParticleConstants>>m_gpu;

    //�ۑ�����R���X�^���g�o�b�t�@
    struct GPUparticleSaveConstants
    {
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        DirectX::XMFLOAT4 startcolor = { 1,1,1,1 };
        DirectX::XMFLOAT4 endcolor = { 1,1,1,1 };
        DirectX::XMFLOAT4 shape = { 0,0.0f,0.0f,0.0f }; //���F�����ꏊ�����_���A���F���a�A���F���a�̃{�����[���A���F�~�`�ɂ���p��
        DirectX::XMFLOAT3 velocity = { 0,0,0 };
        float lifetime = 1.0f;
        DirectX::XMFLOAT3 luminance = { 1,1,1 };
        float speed = 1.0f;
        DirectX::XMFLOAT2 scale = { 0.2f,0.2f };
        float startsize = 1.0f;
        float endsize = 1.0f;
        DirectX::XMFLOAT3 orbitalvelocity = { 0,0,0 };
        float radial = { 0 };
        float startspeed = 1.0f;
        float endspeed = 1.0f;
        float velorandscale = 0.0f;
        int strechflag = 0;
        DirectX::XMFLOAT3 buoyancy = {};
        float startgravity = 0.0f;
        float endgravity = 0.0f;
        DirectX::XMFLOAT3 savedummy = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    GPUparticleSaveConstants m_GSC;

    //�ۑ�����p�����[�^
    struct SaveParameter
    {
        int m_blend = 2;
        std::string	m_filename;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameter m_p;

private:
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
};