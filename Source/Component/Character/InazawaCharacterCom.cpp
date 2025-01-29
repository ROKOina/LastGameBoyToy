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
#include "Phsix\Physxlib.h"
#include "Component\Renderer\DecalCom.h"
#include "Audio/Audio3D.h"

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
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::RELOAD, std::make_shared<BaseCharacter_ReloadState>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

void InazawaCharacterCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);

    //ウルトエフェクト
    bool objHitFlag = false;
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
                    Audio2DMagaer::Instance().Audio2DStop(AUDIOID2D::PLAYER_ATTACKULTBOOM);
                    Audio2DMagaer::Instance().Audio2DPlay(AUDIOID2D::PLAYER_ATTACKULTBOOM, 10.0f, false);
                }
                objHitFlag = true;
            }

            if (rayCol->GetEnabled() && !objHitFlag)
            {
                // 判定
                PxRaycastBuffer buffer;
                DirectX::XMFLOAT3 start = rayCol->GetGameObject()->transform_->GetWorldPosition();
                DirectX::XMFLOAT3 end = start + GetFpsCameraDir() * 100;
                if (PhysXLib::Instance().RayCast_PhysX(start, Mathf::Normalize(end - start), Mathf::Length(end - start), buffer, PhysXLib::CollisionLayer::Stage))
                {
                    DirectX::XMFLOAT3 hitPosition;
                    DirectX::XMFLOAT3 hitNormal;
                    // レイキャストが当たった位置と法線を保存
                    hitPosition.x = buffer.block.position.x;
                    hitPosition.y = buffer.block.position.y;
                    hitPosition.z = buffer.block.position.z;
                    hitNormal.x = buffer.block.normal.x;
                    hitNormal.y = buffer.block.normal.y;
                    hitNormal.z = buffer.block.normal.z;

                    //デカール生成
                    std::shared_ptr<GameObject>decal = GameObjectManager::Instance().Create();
                    decal->SetName("decal");
                    std::shared_ptr<Decal>d = decal->AddComponent<Decal>("Data/Texture/bullethole.png");
                    d->Add(hitPosition, hitNormal, 1.0f);
                }
            }
        }
    }
}

void InazawaCharacterCom::OnGUI()
{
    CharacterCom::OnGUI();
    ImGui::DragInt("attackUltCountMax", &attackUltCountMax);
    ImGui::DragInt("attackUltCounter", &attackUltCounter);
}

void InazawaCharacterCom::MainAttackDown()
{
    //スキル発動中はリターン
    if (attackStateMachine.GetCurrentState() == CHARACTER_ATTACK_ACTIONS::SUB_SKILL)return;

    //ウルト発動中
    if (UseUlt())
    {
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::ULT);

        attackUltCounter++;
        if (attackUltCounter >= attackUltCountMax)
        {
            if (std::string(GetGameObject()->GetName()) == "player")
            {
                //エフェクト切る
                GameObjectManager::Instance().Find("attackUltSide1")->GetComponent<GPUParticle>()->SetLoop(false);
                GameObjectManager::Instance().Find("attackUltSide2")->GetComponent<GPUParticle>()->SetLoop(false);
            }
            FinishUlt();
        }

        //腕アニメーション再生
        HandleArmAnimation();

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
        ResetSkillCoolTimer(SkillCoolID::E);
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

    if (std::string(GetGameObject()->GetName()) == "player")
    {
        //エフェクト起動
        GameObjectManager::Instance().Find("attackUltSide1")->GetComponent<GPUParticle>()->SetLoop(true);
        GameObjectManager::Instance().Find("attackUltSide2")->GetComponent<GPUParticle>()->SetLoop(true);
    }

    SetMaxBullet();

    //ステートを初期化
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}