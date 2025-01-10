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

    //攻撃ウルト取得
    void SetAttackRayObj(std::shared_ptr<GameObject> obj) { attackray = obj; }

private:

    //ヒットスキャンが当たった時の処理
    void HitObject();

    //銃口にエフェクトを付ける
    void SetMuzzleFlash();

private:

    //ヒットスキャンのオブジェクト
    std::weak_ptr<GameObject> attackray;
};