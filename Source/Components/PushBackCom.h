#pragma once

#include "System\Component.h"

//�����Ԃ�����R���|�[�l���g

//PushBackCom
class PushBackCom : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    PushBackCom();
    ~PushBackCom() {}

    // ���O�擾
    const char* GetName() const override { return "PushBackCom"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    // debug�`��
    void DebugRender();

    void ResetHitFlg() { isHit = false; }

    void PushBackUpdate(std::shared_ptr<PushBackCom> otherSide);

    //�d���ݒ�
    void SetWeight(float weight) { weight_ = weight; }
    float GetWeight() { return weight_; }
    //���a�ݒ�
    void SetRadius(float radius) { radius_ = radius; }
    float GetRadius() { return radius_; }
private:
    bool isHit = false;

    float radius_ = 0.5f;

    //�d��
    float weight_ = 1;
};