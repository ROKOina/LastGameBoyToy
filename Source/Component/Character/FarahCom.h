#pragma once

#include "CharacterCom.h"
#include <algorithm>
#include <vector>
#include <memory>

class FarahCom : public CharacterCom
{
public:

    // 個々の弾丸情報を表す構造体
    struct FarahBullet
    {
        float bombertimer = 0.0f;          // 爆発タイマー
        bool bomberflag = false;           // 爆発フラグ
        bool played = false;
        float rotation = 0;
        std::shared_ptr<GameObject> obj;   // 弾丸オブジェクトへのポインタ
    };

public:
    // 名前取得
    const char* GetName() const override { return "FarahCom"; }

    // 初期化
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI
    void OnGUI() override;

    // 右クリック単発押し処理
    void SubAttackDown() override;

    // スペーススキル長押し
    void SpaceSkillPushing(float elapsedTime) override;

    // Eスキル
    void SubSkill() override;

    // メイン攻撃
    void MainAttackDown() override;

    // ULT
    void UltSkill() override;

    //リロード（弾減らす処理は各自のキャラでする
    void Reload()override;

private:

    // ウルト更新
    void UltUpdate(float elapsedTime);

    // 銃の打つ間隔とマズルフラッシュ
    void ShotSecond();

    // ウルトクールダウン管理
    void HandleCooldown(float elapsedTime);

    // ブーストフラグ管理
    void HandleBoostFlag();

    // ジャンプ処理
    void ApplyJumpForce();

    // クールダウン設定
    void SetCooldown(float time);

    // ダッシュゲージ増加
    void AddDashGauge(float amount);

    // ウルトリセット
    void ResetUlt();

    // 地面で爆発処理
    void GroundBomber(float elapsedTime);

    // 弾丸削除
    void RemoveBullet(const std::shared_ptr<GameObject>& obj);

    // 全弾削除
    void ClearAllBullets();

public:

    // ゲージ減少速度設定
    void SetDashGaugeMins(const float& dashgaugemin_) { dashgaugemin = dashgaugemin_; }

    // 弾丸生成
    void AddBullet(const std::shared_ptr<GameObject>& obj);

private:
    float cooldownTimer = 0.0f;  // クールタイムの残り時間（秒）
    float ulttimer = 0.0f;       // ウルト時間
    float dashgaugemin = 4.0f;   // ダッシュゲージの最小値
    std::vector<FarahBullet> bullets;  // 弾丸リスト
};