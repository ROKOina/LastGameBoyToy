#pragma once

#include "../System\Component.h"

class OnGroundDeleteCom : public Component
{
public:
    OnGroundDeleteCom() {};
    ~OnGroundDeleteCom() override {};

    // ���O�擾
    const char* GetName() const override { return "OnGroundDeleteCom"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

private:

    //�X�e�[�W�ɓ�����΍폜����֐�
    void IsGroundDelete();

    //�q�b�g�����ʒu�Ɩ@�����擾���Đݒ�
    void IsRayCast();
};