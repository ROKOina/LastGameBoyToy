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

    //fps用の腕アニメーション
    void FPSArmAnimation();

public:

    //ウルト更新
    void UltUpdate(float elapsedTime);

public:

    //ゲージ減る速度セット
    void SetDashuGaugeMins(const float& dashgaugemin_) { dashgaugemin = dashgaugemin_; }

private:
    float cooldownTimer = 0.0f;  // クールタイムの残り時間（秒）
    float dashgaugemin = 4.0f;
    float ulttimer = 0.0f;
};