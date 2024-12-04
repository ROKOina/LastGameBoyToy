#pragma once

#include "../System\Component.h"

class KnockBackCom : public Component
{
public:
    KnockBackCom() {};
    ~KnockBackCom() override {};

    // ���O�擾
    const char* GetName() const override { return "KnockBack"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

public:

    //�m�b�N�o�b�N�̌W��
    void SetKnockBackForce(DirectX::XMFLOAT3& knockbackforce_) { knockbackforce = knockbackforce_; }

private:

    DirectX::XMFLOAT3 knockbackforce = {};
};