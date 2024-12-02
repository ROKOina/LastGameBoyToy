#pragma once

#include "CharacterCom.h"

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
};