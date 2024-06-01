#pragma once

#include "Graphics/constant_buffer.h"
#include <DirectXMath.h>
#include <memory>

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
    virtual void Render() = 0;

    //viewport�̐ݒ�
    void ViewPortInitialize();

    //�V�[���̃R���X�^���g�o�b�t�@�̍X�V
    void ConstantBufferUpdate();

    //�V�[���̃R���X�^���g�o�b�t�@�̏�����
    void ConstantBufferInitialize();

    //�����������Ă��邩
    bool IsReady()const { return ready; }

    //���������ݒ�
    void SetReady() { ready = true; }

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
        float dummy = {};
    };
    std::unique_ptr<constant_buffer<SceneConstants>>sc;

protected:
    bool ready = false;
};