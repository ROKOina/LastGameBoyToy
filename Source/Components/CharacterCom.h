#pragma once
#include "System\Component.h"

class CharacterCom : public Component
{
    CharacterCom() {};
    ~CharacterCom() override {};

    // ���O�擾
    const char* GetName() const override { return "Character"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

    // �R���g���[���[����i�s�����擾
    DirectX::XMFLOAT3 CalcMoveVec() const;

};
