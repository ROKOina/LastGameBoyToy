#include "InazawaCharacterCom.h"
#include "StateMachine\Behaviar\InazawaCharacterState.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "Scene/SceneManager.h"
#include "Component\Camera\CameraCom.h"
#include "Component\Renderer\RendererCom.h"
#include "CharaStatusCom.h"
#include "Component\UI\PlayerUI.h"
#include "Component\Sprite\Sprite.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\RemoveTimerCom.h"
#include "Component\Audio\AudioCom.h"

void InazawaCharacterCom::Start()
{
    //ステート登録
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<BaseCharacter_JumpLoop>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::LANDING, std::make_shared<BaseCharacter_Landing>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<InazawaCharacter_AttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<InazawaCharacter_ESkillState>(this));
    //ウルト追加
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::ULT, std::make_shared<Ult_Attack_State>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

void InazawaCharacterCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);

    FPSArmAnimation();

    shootTimer += elapsedTime;
    //攻撃先行入力
    if (attackInputSave)
    {
        if (shootTimer >= shootTime)
        {
            //スキル発動中はリターン
            if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::SUB_SKILL)
            {
                attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
            }
            attackInputSave = false;
        }
        if (CharacterInput::MainAttackButton & GetButtonUp())
            attackInputSave = false;
    }

    //ウルトエフェクト
    if (attackUltRayObj.lock())
    {
        auto& rayCol = attackUltRayObj.lock()->GetComponent<Collider>();
        if (rayCol)
        {
            for (auto& obj : rayCol->OnHitGameObject())
            {
                {
                    std::shared_ptr<GameObject> attackUltEffBomb = GameObjectManager::Instance().Create();
                    attackUltEffBomb->SetName("attackUltEffBomb");
                    attackUltEffBomb->transform_->SetWorldPosition(obj.hitPos);
                    std::shared_ptr<GPUParticle> eff = attackUltEffBomb->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltBombCircle.gpuparticle", 300);
                    eff->Play();
                    attackUltEffBomb->AddComponent<RemoveTimerCom>(3);
                    {
                        std::shared_ptr<GameObject> attackUltEffBomb02 = attackUltEffBomb->AddChildObject();
                        attackUltEffBomb02->SetName("attackUltEffBomb02");
                        std::shared_ptr<GPUParticle> eff = attackUltEffBomb02->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltBombCircleLight.gpuparticle", 100);
                        eff->Play();
                    }
                    {
                        std::shared_ptr<GameObject> attackUltEffBomb03 = attackUltEffBomb->AddChildObject();
                        attackUltEffBomb03->SetName("attackUltEffBomb03");
                        std::shared_ptr<GPUParticle> eff = attackUltEffBomb03->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltBombFire.gpuparticle", 50);
                        eff->Play();
                    }

                    //音
                    GetGameObject()->GetComponent<AudioCom>()->Stop("P_ATTACK_ULT_BOOM");
                    GetGameObject()->GetComponent<AudioCom>()->Play("P_ATTACK_ULT_BOOM", false, 10);
                }
            }
        }
    }
}

void InazawaCharacterCom::OnGUI()
{
    CharacterCom::OnGUI();
    ImGui::DragFloat("shootTime", &shootTime);
    ImGui::DragFloat("shootTimer", &shootTimer);

    ImGui::DragInt("attackUltCountMax", &attackUltCountMax);
    ImGui::DragInt("attackUltCounter", &attackUltCounter);
}

void InazawaCharacterCom::MainAttackDown()
{
    //スキル発動中はリターン
    if (attackStateMachine.GetCurrentState() == CHARACTER_ATTACK_ACTIONS::SUB_SKILL)return;

    ////発砲アニメーションの場合はリターン
    //auto& arm = GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
    //auto& armAnim = arm->GetComponent<AnimationCom>();
    //if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_shoot"))
    //{
    //    attackInputSave = true; //先行入力保存
    //    return;
    //}

    //ウルト発動中
    if (UseUlt())
    {
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::ULT);

        attackUltCounter++;
        if (attackUltCounter >= attackUltCountMax)
        {
            //エフェクト切る
            GameObjectManager::Instance().Find("attackUltSide1")->GetComponent<GPUParticle>()->SetLoop(false);
            GameObjectManager::Instance().Find("attackUltSide2")->GetComponent<GPUParticle>()->SetLoop(false);
            FinishUlt();
        }

        return;
    }

    if (shootTimer < shootTime)
    {
        attackInputSave = true; //先行入力保存
        return;
    }

    //アタック
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
    attackInputSave = false;
}

//ブリンク
void InazawaCharacterCom::SubAttackDown()
{
    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec(GetGameObject());

    //ダッシュ
    auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 v = moveVec * 60.0f;
    moveCom->AddNonMaxSpeedForce(v);
}

void InazawaCharacterCom::SubSkill()
{
    if (!UseUlt())
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL);
    else
        ResetESkillCool();
}

void InazawaCharacterCom::UltSkill()
{
    //int counter = *GetRCounter();
    //if (counter >= 0 && counter <= 4)
    //{
    //    std::string coreName = "core" + std::to_string(4 - counter);
    //    const auto& sprite = GameObjectManager::Instance().Find(coreName.c_str())->GetComponent<Sprite>();
    //    if (sprite)
    //    {
    //        sprite->EasingPlay();
    //    }
    //}

    attackUltCounter = 0;

    //エフェクト起動
    GameObjectManager::Instance().Find("attackUltSide1")->GetComponent<GPUParticle>()->SetLoop(true);
    GameObjectManager::Instance().Find("attackUltSide2")->GetComponent<GPUParticle>()->SetLoop(true);
    //ステートを初期化
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);

}

//終わってます。何故ステートマシンか何かを作らなかったの？(不思議でしかたない)by上野
void InazawaCharacterCom::FPSArmAnimation()
{
    if (std::string(GetGameObject()->GetName()) != "player")return;
    auto& arm = GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
    auto& armAnim = arm->GetComponent<AnimationCom>();

    //待機
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::IDLE)
    {
        if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_idol"))return;

        if (armAnim->GetCurrentAnimationIndex() != armAnim->FindAnimation("FPS_shoot"))
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_idol"), true);
    }

    //移動
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::MOVE)
    {
        if (armAnim->GetCurrentAnimationIndex() != armAnim->FindAnimation("FPS_walk"))
        {
            if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_shoot"))
            {
                if (armAnim->IsEventCalling("attackEnd"))
                    armAnim->PlayAnimation(armAnim->FindAnimation("FPS_walk"), true);
            }
            else
                armAnim->PlayAnimation(armAnim->FindAnimation("FPS_walk"), true);
        }
    }

    //アニメーションスピード変更
    float fmax = GetGameObject()->GetComponent<MovementCom>()->GetFisrtMoveMaxSpeed();
    float max = GetGameObject()->GetComponent<MovementCom>()->GetMoveMaxSpeed();

    float v = max - fmax;
    if (v < 0)v = 0;

    arm->GetComponent<RendererCom>()->GetModel()->GetResource()->GetAnimationsEdit()[armAnim->FindAnimation("FPS_walk")].animationspeed
        = 1 + v * 0.1f;
}