#pragma once
#include "Component/System/Component.h"


class UltSkillMaxItem :public Component
{
public:

    UltSkillMaxItem() {};
    ~UltSkillMaxItem() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //IMGUI
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "ULTSKILLMAX"; }


public:

    //アイテムを拾った時の処理
    void OnItemPickup();

    //アイテムの動き
    void TransformItem(float elapsedTime);

    float timer = 0.0f;
  
};