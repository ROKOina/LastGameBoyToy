#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"

//aimikcom
class AimIKCom : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    AimIKCom(const char* ainbonename);
    ~AimIKCom() {}

    // ���O�擾
    const char* GetName() const override { return "AimIK"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

private:

    //AimIK�֐�
    void AimIK();

    //ik������bone��T��
    void SearchAimNode(const char* ainbonename);

private:

    //AimIK�p�ϐ�
    std::vector<int>AimBone;
    const char* copyname = {};
};