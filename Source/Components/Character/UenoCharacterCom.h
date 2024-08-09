#pragma once
#include "CharacterCom.h"
#include "Components/GPUParticle.h"

class UenoCharacterCom : public CharacterCom
{
public:
    UenoCharacterCom();
    ~UenoCharacterCom() {}

    // ���O�擾
    const char* GetName() const override { return "UenoCharacter"; }

    //������
    void Start() override;

    //�X�V����
    void Update(float elapsedTime) override;

    //imgui
    void OnGUI() override;

    //���C���A�^�b�N
    void MainAttackDown()override;

public:

    void SetLazerFlag(const bool& lazer) { lazerflag = lazer; }
    const bool& GetLazerFlag()const { return lazerflag; }

public:

    std::shared_ptr<GPUParticle>gpulazerparticle;

private:

    //�p�[�e�B�N���X�V
    void LazerParticleUpdate(float elapsedTime);

private:
    bool lazerflag = false;
};