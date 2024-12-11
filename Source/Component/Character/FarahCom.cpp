#include "FarahCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "Component\Renderer\RendererCom.h"
#include "Math\Mathf.h"
#include "SystemStruct\TimeManager.h"
#include "StateMachine\Behaviar\FarahState.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\Collsion\ColliderCom.h"

// 定数
constexpr float JUMP_FORCE = 12.62f;
constexpr float RISING_FORCE = 13.0f;
constexpr float COOLDOWN_TIME = 0.5f;
constexpr float DASH_GAUGE_INCREMENT = 5.0f;
constexpr float ULT_DURATION = 15.0f;

// 初期化
void FarahCom::Start()
{
    // ステート登録 (移動)
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<BaseCharacter_JumpLoop>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::LANDING, std::make_shared<BaseCharacter_Landing>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    // ステート登録 (攻撃)
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<Farah_MainAttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<Farah_ESkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::ULT, std::make_shared<Farah_UltState>(this));

    // 初期ステート設定
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

// 更新処理
void FarahCom::Update(float elapsedTime)
{
    HandleCooldown(elapsedTime);
    HandleBoostFlag();
    UltUpdate(elapsedTime);
    ShotSecond();
    CharacterCom::Update(elapsedTime);
    FPSArmAnimation();
    GroundBomber(elapsedTime);
}

// GUI
void FarahCom::OnGUI()
{
    ImGui::DragFloat("Cooldown Timer", &cooldownTimer);
    CharacterCom::OnGUI();
}

// サブ攻撃 (右クリック)
void FarahCom::SubAttackDown()
{
    if (cooldownTimer > 0.0f) return;

    ApplyJumpForce();
    SetCooldown(COOLDOWN_TIME);
    AddDashGauge(DASH_GAUGE_INCREMENT);
}

// スペーススキル (長押し)
void FarahCom::SpaceSkillPushing(float elapsedTime)
{
    if (cooldownTimer > 0.0f || dashGauge <= 0.01f) return;

    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::JUMPLOOP)
    {
        dashGauge -= dashgaugemin * elapsedTime;
        GetGameObject()->GetComponent<MovementCom>()->Rising(elapsedTime);
    }
}

// Eスキル
void FarahCom::SubSkill()
{
    if (!UseUlt())
    {
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL);
    }
    else
    {
        ResetSkillCoolTimer(SkillCoolID::E);
    }
}

// メイン攻撃
void FarahCom::MainAttackDown()
{
    //スキル発動中はリターン
    if (attackStateMachine.GetCurrentState() == CHARACTER_ATTACK_ACTIONS::SUB_SKILL)return;

    //アタック
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
    attackInputSave = false;
}

// ウルトスキル
void FarahCom::UltSkill()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::ULT);
}

// ウルト更新
void FarahCom::UltUpdate(float elapsedTime)
{
    if (!UseUlt()) return;

    ulttimer += elapsedTime;
    if (ulttimer > ULT_DURATION)
    {
        ResetUlt();
    }
}

// 銃の発射間隔とマズルフラッシュ
void FarahCom::ShotSecond()
{
    const auto& arm = GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
    const auto& particle = arm->GetChildFind("muzzleflash");
    DirectX::XMFLOAT3 pos;

    bool isNodeActive = arm->GetComponent<AnimationCom>()->IsEventCallingNodePos("MUZZLEFLASH", "gun2", pos);
    particle->transform_->SetWorldPosition(pos);
    particle->GetComponent<CPUParticle>()->SetActive(isNodeActive);
}

// ジャンプ力を適用
void FarahCom::ApplyJumpForce()
{
    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    moveCom->SetAirForce(JUMP_FORCE);
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::JUMP);

    DirectX::XMFLOAT3 velocity = moveCom->GetVelocity();
    velocity.y = 0.0f;
    moveCom->SetVelocity(velocity);

    DirectX::XMFLOAT3 power = { 0.0f, Mathf::Lerp(0.0f, RISING_FORCE, 0.8f), 0.0f };
    moveCom->AddForce(power);
}

// クールダウンを設定
void FarahCom::SetCooldown(float time)
{
    cooldownTimer = time;
}

// ダッシュゲージを増加
void FarahCom::AddDashGauge(float amount)
{
    dashGauge += amount;
}

// ウルトリセット
void FarahCom::ResetUlt()
{
    dashgaugemin = 4.0f;
    GetGameObject()->GetComponent<MovementCom>()->SetMoveAcceleration(3.0f);
    FinishUlt();
    ulttimer = 0.0f;
}

// クールダウンの管理
void FarahCom::HandleCooldown(float elapsedTime)
{
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer = (std::max)(0.0f, cooldownTimer - elapsedTime);
    }
}

// ブーストフラグ管理
void FarahCom::HandleBoostFlag()
{
    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    boostflag = !moveCom->OnGround();
}

// 地面で爆発処理
void FarahCom::GroundBomber(float elapsedTime)
{
    for (auto& bullet : bullets)
    {
        if (!bullet.obj) continue;

        // 地面に接触
        auto& obj = bullet.obj;
        if (obj->GetComponent<MovementCom>()->OnGround() || obj->GetComponent<MovementCom>()->GetOnWall())
        {
            bullet.bomberflag = true;
            obj->GetComponent<MovementCom>()->SetIsRaycast(false);
            obj->GetComponent<MovementCom>()->ZeroVelocity();
            obj->GetComponent<MovementCom>()->ZeroNonMaxSpeedVelocity();
            obj->GetComponent<SphereColliderCom>()->SetRadius(2.1f);
        }

        // 爆発フラグ処理
        if (bullet.bomberflag)
        {
            bullet.bombertimer += elapsedTime;

            // 一度だけ Play を実行する
            if (!bullet.played)  // played フラグで制御
            {
                obj->GetComponent<GPUParticle>()->SetEnabled(true);
                obj->GetComponent<GPUParticle>()->Play();
                bullet.played = true; // フラグを設定
            }

            //回転させる
            bullet.rotation += elapsedTime * 900;
            obj->GetComponent<GPUParticle>()->GetGameObject()->transform_->SetEulerRotation({ 0.0f,0.0f,bullet.rotation });
        }

        // 爆発後に削除
        if (bullet.bombertimer >= 0.1f)
        {
            auto& viewBullet = obj->GetComponent<BulletCom>()->GetViewBullet().lock();
            if (viewBullet)
            {
                GameObjectManager::Instance().Remove(viewBullet);
            }

            if (bullet.bombertimer > 0.5f)
            {
                GameObjectManager::Instance().Remove(obj);
                RemoveBullet(obj);
            }
        }
    }
}

// 弾丸削除
void FarahCom::RemoveBullet(const std::shared_ptr<GameObject>& obj)
{
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [&obj](const FarahBullet& bullet) {
            return bullet.obj == obj;
        }),
        bullets.end());
}

// 全弾削除
void FarahCom::ClearAllBullets()
{
    bullets.clear();
}

// 弾丸生成
void FarahCom::AddBullet(const std::shared_ptr<GameObject>& obj)
{
    FarahBullet bullet;
    bullet.obj = obj;
    bullets.push_back(bullet);
}