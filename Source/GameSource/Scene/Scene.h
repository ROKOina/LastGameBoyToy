#pragma once

#include "Graphics/Shaders/ConstantBuffer.h"
#include <DirectXMath.h>
#include <memory>

//�O���錾
class CameraCom;

//�V�[��
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    //������
    virtual void Initialize() = 0;

    //�I����
    virtual void Finalize() = 0;

    //�X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    virtual void Render(float elapsedTime) = 0;

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
        DirectX::XMFLOAT3 cameraposition = {};
        float time = {};
        float deltatime = {};
        DirectX::XMFLOAT3 SCdummy = {};
    };
    std::unique_ptr<ConstantBuffer<SceneConstants>>sc;

private:
    bool isReady_ = false;
};