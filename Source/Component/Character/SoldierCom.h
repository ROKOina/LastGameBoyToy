#pragma once

#include "CharacterCom.h"

class SoldierCom : public CharacterCom
{
public:

    // 名前取得
    const char* GetName() const override { return "SoldierCom"; }

    // 初期化
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 右クリック単発押し処理
    void SubAttackDown() override;

    // Eスキル
    void SubSkill() override;

    // メイン攻撃
    void MainAttackDown() override;

    // ULT
    void UltSkill() override;

    //リロード（弾減らす処理は各自のキャラでする
    void Reload()override;

    // GUI
    void OnGUI() override;
};