#pragma once
#include "Component/System/Component.h"


class UltSkillMaxItem :public Component
{
public:

    UltSkillMaxItem() {};
    ~UltSkillMaxItem() {};

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //IMGUI
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "ULTSKILLMAX"; }


public:

    //�A�C�e�����E�������̏���
    void OnItemPickup();

    //�A�C�e���̓���
    void TransformItem(float elapsedTime);

    float timer = 0.0f;
  
};