#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"

//aimikcom
class AimIKCom : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    AimIKCom(const char* playeraimbonename, const char* enemyaimbone);
    ~AimIKCom() {}

    // ���O�擾
    const char* GetName() const override { return "AimIK"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override {};

    // GUI�`��
    void OnGUI() override;

    //AimIK�֐�
    void AimIK();

private:

    //ik������bone��T��
    void SearchAimNode(const char* aimbonename);

private:

    //AimIK�p�ϐ�
    std::vector<int>AimBone;
    const char* playercopyname = {};
    const char* enemycopyname = {};
    DirectX::XMFLOAT4 neckpos = { 0,0,0,1 };
};