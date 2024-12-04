#pragma once

#include "CharacterCom.h"
#include <algorithm>

class FarahCom : public CharacterCom
{
public:
    // 名前取得
    const char* GetName() const override { return "FarahCom"; }

    //初期化
    void Start() override;

    //更新処理
    void Update(float elapsedTime) override;

    //gui
    void OnGUI() override;

    //右クリック単発押し処理
    void SubAttackDown() override;

    //スペーススキル長押し
    void SpaceSkillPushing(float elapsedTime) override;

    //Eスキル
    void SubSkill() override;

    //メインの攻撃
    void MainAttackDown() override;

    //ULT
    void UltSkill()override;

private:

    //fps用の腕アニメーション
    void FPSArmAnimation();

private:
    float cooldownTimer = 0.0f;  // クールタイムの残り時間（秒）
    float dashgaugemin = 4.0f;
};