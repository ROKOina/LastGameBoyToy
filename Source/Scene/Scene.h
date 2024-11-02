#pragma once

#include "Graphics/ConstantBuffer.h"
#include <DirectXMath.h>
#include <memory>
#include <string>

//�O���錾
class CameraCom;

//�V�[��
class Scene
{
private:
    // �R�s�[�R���X�g���N�^�h�~
    Scene(const Scene&) = delete;
    Scene(Scene&&)  noexcept = delete;
    Scene& operator=(Scene const&) = delete;
    Scene& operator=(Scene&&) noexcept = delete;

public:
    Scene();
    virtual ~Scene() {}

    //������
    virtual void Initialize() = 0;

    //�I����
    virtual void Finalize() = 0;

    //�X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    virtual void Render(float elapsedTime) = 0;

    //���O�擾
    virtual std::string GetName() const = 0;

    //�����������Ă��邩
    bool IsReady()const { return isReady_; }

    //���������ݒ�
    void SetReady() { isReady_ = true; }

    //�V�[���̃R���X�^���g�o�b�t�@�̍X�V
    void ConstantBufferUpdate(float elapsedTime);

    //�V�[���̃R���X�^���g�o�b�t�@�̏�����
    void ConstantBufferInitialize();

protected:
    //�V�[���̃R���X�^���g�o�b�t�@�̍\����
    struct SceneConstants
    {
        DirectX::XMFLOAT4X4 view = {};
        DirectX::XMFLOAT4X4 projection = {};
        DirectX::XMFLOAT4X4 viewprojection = {};
        DirectX::XMFLOAT4X4 inverseview = {};
        DirectX::XMFLOAT4X4 inverseprojection = {};
        DirectX::XMFLOAT4X4 inverseviewprojection = {};
        DirectX::XMFLOAT3 cameraposition = {};
        float time = {};
        DirectX::XMFLOAT3 bossposiotn = {};
        float deltatime = {};
        DirectX::XMFLOAT2 cameraScope = {};
        DirectX::XMFLOAT2 screenResolution = {};
    };
    std::unique_ptr<ConstantBuffer<SceneConstants>>sc;

private:
    bool isReady_ = false;
};