#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"

class InstanceTransform : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    InstanceTransform() {};
    ~InstanceTransform() {}

    // ���O�擾
    const char* GetName() const override { return "InstanceTransform"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

private:

    //�C���X�^���V���O�̏��
    struct Instance
    {
        DirectX::XMFLOAT4 quaternion = { 0,0,0,1 };
        DirectX::XMFLOAT3 position{ 0,0,0 };
        DirectX::XMFLOAT3 scale{ 1.0f,1.0f,1.0f };
    };
    std::unique_ptr<Instance[]> m_cpuinstancedata;
};