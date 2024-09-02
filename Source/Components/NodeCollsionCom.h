#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"

//�A�j���[�V����
class NodeCollsionCom : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    NodeCollsionCom(const char* filename);
    ~NodeCollsionCom() {}

    // ���O�擾
    const char* GetName() const override { return "NodeCollsion"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

private:

    //�f�o�b�O�v���~�e�B�u�̌`��
    enum class CollsionType
    {
        SPHER,
        CYLINDER,
        MAX
    };
    CollsionType m_collsiontype = CollsionType::SPHER;

private:

    const char* m_filename = {};
    Model* model = nullptr;
    bool debugrender = false;
};