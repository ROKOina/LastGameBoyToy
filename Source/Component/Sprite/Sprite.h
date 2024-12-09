#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Component/System/Component.h"
#include "Component/Camera/CameraCom.h"

// �X�v���C�g
class Sprite :public Component
{
public:

    //�V�F�[�_�[�R�[�h��ύX����
    enum class SpriteShader
    {
        DEFALT,
        BLUR,
        DISSOLVE,
        CHROMATICABERRATION,
        MAX
    };

public:

    Sprite(const char* filename, SpriteShader spriteshader, bool collsion);
    ~Sprite() {}

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //�`��
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    //imgui
    void OnGUI()override;

    void LoadTexture(std::string filename);

    //���O�ݒ�
    const char* GetName() const override { return "Sprite"; }

    //�C�[�W���O�v���C�֐�
    void EasingPlay();

    //�C�[�W���O��~
    void StopEasing();



private:

    //�V���A���C�Y
    void Serialize();

    // �f�V���A���C�Y
    void Deserialize(const char* filename);

    // �f�V���A���C�Y�̓ǂݍ���
    void LoadDeserialize();

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
        DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
        DirectX::XMFLOAT4 easingcolor = { 1.0f,1.0f,1.0f,1.0f };
        DirectX::XMFLOAT2 position = { 0.0f,0.0f };
        DirectX::XMFLOAT2 easingposition = { 0.0f,0.0f };
        DirectX::XMFLOAT2 scale = { 0.0f,0.0f };
        DirectX::XMFLOAT2 easingscale = { 0.0f,0.0f };
        DirectX::XMFLOAT2 collsionscaleoffset = { 0.0f,0.0f };
        DirectX::XMFLOAT2 collsionpositionoffset = { 0.0f,0.0f };
        DirectX::XMFLOAT2 pivot = { 0.0f,0.0f };
        DirectX::XMFLOAT2 texSize = { 0.0f,0.0f };
        float angle = { 0.0f };
        float easingangle = { 0.0f };
        std::string	filename = {};
        int blend = 1;
        int depth = 1;
        float timescale = 0.0f;
        int easingtype = 0;
        int easingmovetype = 0;
        bool loop = false;
        bool comback = false;
        bool easing = false;
        std::string objectname = {};
        DirectX::XMFLOAT3 screenposoffset = { 0.0f,0.0f,0.0f };
        DirectX::XMFLOAT2 maxscale = { 0.0f,0.0f };
        DirectX::XMFLOAT2 minscale = { 0.0f,0.0f };
        bool onshot = false;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameterCPU spc = {};

    //�萔�o�b�t�@�̍\����
    struct SaveConstantsParameter
    {
        DirectX::XMFLOAT3 blurcolor = { 1.0f,1.0f,1.0f };
        float blurdistance = { 8.0f };
        float blurpower = { 0.1f };
        float luminance = { 0.0f };
        DirectX::XMFLOAT2 uvscroll = { 0.0f,0.0f };
        DirectX::XMFLOAT3 edgecolor = { 1,1,1 };
        float cliptime = 0.0f;
        float edgethreshold = 1.0f;
        float edgeoffset = 0.0f;
        DirectX::XMFLOAT2 padding = {};
    };
    SaveConstantsParameter constants;

    //�J�[�\�����X�v���C�g�ɓ������Ă��邩
    bool GetHitSprite() { return hit; }
    //�J�[�\�����X�v���C�g�ɓ��������u��
    bool GetHitSpriteEnter() { return hitEnter; }

    //�N���b�v�^�C���Z�b�^�[�A�Q�b�^�[
    void SetClipTime(float cliptime) { constants.cliptime = cliptime; }
    float GetClipTime() { return constants.cliptime; }

    //�����p�ɒǉ�
    DirectX::XMFLOAT2 numUVScroll = { 0,0 };

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
    D3D11_TEXTURE2D_DESC collisionTexture2ddesc_ = {};
    D3D11_TEXTURE2D_DESC rampTexture2ddesc_ = {};
    D3D11_TEXTURE2D_DESC dissolveTexture2ddesc_ = {};
    DirectX::XMFLOAT2 savepos = {};
    DirectX::XMFLOAT4 savecolor = { 1,1,1,1 };
    DirectX::XMFLOAT2 savescale = {};
    float saveangle = {};
    float easingresult = 0.0f;
    float easingtime = 0.0f;
    bool play = false;
    bool loopon = false;
    bool drawcollsion = false;
    bool hit = false;
    bool hitEnter = false;
    bool ontriiger = false;

    DirectX::XMFLOAT2 collisionPivot;
};