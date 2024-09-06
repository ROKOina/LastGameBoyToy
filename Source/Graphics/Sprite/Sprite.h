#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Components/System/Component.h"
#include "Components/CameraCom.h"

// �X�v���C�g
class Sprite :public Component
{
public:

    Sprite(const char* filename, bool collsion);
    ~Sprite() {}

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override;

    //�`��
    void Render();

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Sprite"; }

private:

    //�V���A���C�Y
    void Serialize();

    // �f�V���A���C�Y
    void Deserialize(const char* filename);

    // �f�V���A���C�Y�̓ǂݍ���
    void LoadDeserialize();

    //�C�[�W���O��~
    void StopEasing();

    //�����蔻��p�Z�`
    void DrawCollsionBox();

    //�}�E�X�J�[�\���ƃR���W�����{�b�N�X�̓����蔻��
    bool cursorVsCollsionBox();

    //���[����
    void EasingSprite();

public:

    //�ۑ�����p�����[�^
    struct SaveParameterCPU
    {
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        DirectX::XMFLOAT4 easingcolor = { 1,1,1,1 };
        DirectX::XMFLOAT2 position = {};
        DirectX::XMFLOAT2 easingposition = {};
        DirectX::XMFLOAT2 scale = {};
        DirectX::XMFLOAT2 easingscale = {};
        DirectX::XMFLOAT2 collsionscaleoffset = {};
        DirectX::XMFLOAT2 collsionpositionoffset = {};
        float angle = {};
        float easingangle = {};
        std::string	filename;
        int blend = 1;
        int depth = 1;
        float timescale = 0.0f;
        int easingtype = 0;
        int easingmovetype = 0;
        bool loop = false;
        bool comback = false;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameterCPU spc;

    //�萔�o�b�t�@�̍\����
    struct SaveConstantsParameter
    {
        DirectX::XMFLOAT2 uvscroll = { 0.0f,0.0f };
        float cliptime = 0.0f;
        float edgethreshold = 0.6f;
        float edgeoffset = 0.1f;
        DirectX::XMFLOAT3 edgecolor = { 1,1,1 };
        DirectX::XMFLOAT4X4 world = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
    };
    SaveConstantsParameter constants;

private:

    //���_�\����
    struct Vertex
    {
        DirectX::XMFLOAT3	position;
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT2	texcoord;
    };

private:

    Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	collsionshaderResourceView_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	noiseshaderresourceview_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	rampshaderresourceview_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	easingshaderresourceview_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantbuffer;
    D3D11_TEXTURE2D_DESC texture2ddesc_ = {};
    DirectX::XMFLOAT2 savepos = {};
    DirectX::XMFLOAT4 savecolor = {};
    DirectX::XMFLOAT2 savescale = {};
    float saveangle = {};
    float easingresult = 0.0f;
    float easingtime = 0.0f;
    bool play = false;
    bool loopon = false;
    bool drawcollsion = false;
    bool hit = false;
    bool ontriiger = false;
    bool easingsprite = false;
};