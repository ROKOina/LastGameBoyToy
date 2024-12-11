#include "JankratCharacterCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "StateMachine\Behaviar\JankratCharacterState.h"
#include "Component\SkillObj\JankratMineCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Particle\CPUParticle.h"

void JankratCharacterCom::Start()
{
    //ステート登録
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<BaseCharacter_JumpLoop>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::LANDING, std::make_shared<BaseCharacter_Landing>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<JankratCharacter_MainAtkState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_SKILL, std::make_shared<JankratCharacter_MainSkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK, std::make_shared<JankratCharacter_SubAttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::ULT, std::make_shared<JankratCharacter_UltState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);

    //SE登録
}

void JankratCharacterCom::Update(float elapsedTime)
{
    //銃の打つ間隔とマゼルフラッシュ
    ShotSecond();

    //更新
    CharacterCom::Update(elapsedTime);

    //fps用の腕アニメーション
    FPSArmAnimation();

    //後処理
    EraseHaveObjects();
}

void JankratCharacterCom::MainAttackDown()
{
    //スキル発動中はリターン
    if (attackStateMachine.GetCurrentState() == CHARACTER_ATTACK_ACTIONS::SUB_SKILL)return;
    if (attackStateMachine.GetCurrentState() == CHARACTER_ATTACK_ACTIONS::ULT)return;

    //弾撃つ
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
    attackInputSave = false;
}

//ESKILL
void JankratCharacterCom::SubSkill()
{
    if (!UseUlt())
    {
        //地雷設置
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_SKILL);
    }
    else
    {
        ResetSkillCoolTimer(SkillCoolID::E);
    }
}

//ult
void JankratCharacterCom::UltSkill()
{
    //ultステートに遷移
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::ULT);
}

//起爆ステートに遷移
void JankratCharacterCom::SubAttackDown()
{
    //地雷起爆
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);
}

//全てのジャンクラの起爆装置を削除
void JankratCharacterCom::EraseHaveObjects()
{
    std::vector<GameObj> eraseObjs;
    for (int i = 0; i < haveMine.size(); ++i)
    {
        if (haveMine[i]->GetComponent<JankratMineCom>()->GetExplosionFlag())
        {
            eraseObjs.emplace_back(haveMine[i]);
        }
    }
    for (auto& mine : eraseObjs)
    {
        ReleaseHaveMine(mine);
    }
}

//銃の打つ間隔とマゼルフラッシュ
void JankratCharacterCom::ShotSecond()
{
    const auto& arm = GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
    const auto& particle = arm->GetChildFind("muzzleflash");
    DirectX::XMFLOAT3 pos = {};
    if (arm->GetComponent<AnimationCom>()->IsEventCallingNodePos("MUZZLEFLASH", "gun2", pos))
    {
        particle->transform_->SetWorldPosition(pos);
        particle->GetComponent<CPUParticle>()->SetActive(true);
    }
    else
    {
        particle->GetComponent<CPUParticle>()->SetActive(false);
    }
}