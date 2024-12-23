#include "BaseCharacterState.h"
#include "Input\Input.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Character\RemoveTimerCom.h"
#include "Component\Camera\CameraCom.h"
#include <Component\Camera\EventCameraManager.h>
#include "Scene/SceneTitle/SceneTitle.h"
#include "Component\Audio\AudioCom.h"
#include "Component\GameSystem\RespawnCom.h"

//基底君
BaseCharacter_BaseState::BaseCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

//ホバリング
void BaseCharacter_BaseState::Hovering(float elapsedTime)
{
    moveVec = SceneManager::Instance().InputVec(owner->GetGameObject());

    // 空中での速度制御
    const auto& moveComponent = moveCom.lock(); // moveComのロック
    if (moveComponent)
    {
        DirectX::XMFLOAT3 velocity = moveComponent->GetVelocity();

        // ホバリング中の上下方向の速度を管理
        if (velocity.y < 0.05f && HoveringTimer < HoveringTime)
        {
            // 重力の影響を軽減
            velocity.y = -GRAVITY_NORMAL * 0.5f * elapsedTime;

            // ホバリングタイマーを加算
            HoveringTimer += elapsedTime;

            // 微細な上下動を追加（浮遊感を演出）
            float hoverOscillation = sin(HoveringTimer * 3.0f) * 0.1f; // 振幅0.1、周波数3.0
            velocity.y += hoverOscillation;
        }
        else if (HoveringTimer >= HoveringTime)
        {
            // ホバリング終了後、通常の落下挙動
            velocity.y -= GRAVITY_NORMAL * elapsedTime; // 重力を適用
        }

        // 上下方向の速度を更新
        moveComponent->SetVelocity(velocity);

        // 移動力を計算
        DirectX::XMFLOAT3 force =
        {
            moveVec.x * moveComponent->GetMoveAcceleration(),
            0.0f, // 水平方向のみ力を加える
            moveVec.z * moveComponent->GetMoveAcceleration()
        };

        // 移動力を制限して滑らかさを保つ
        float maxForce = 10.0f; // 最大移動力
        force.x = Mathf::Clamp(force.x, -maxForce, maxForce);
        force.z = Mathf::Clamp(force.z, -maxForce, maxForce);

        // 移動力を適用
        moveComponent->AddNonMaxSpeedForce(force);
    }
}

auto MakeNormalLowerAnimeParam = [](int index, bool loop)
    {
        //下半身アニメーション用構造体
        AnimationCom::PlayLowBodyAnimParam param =
        {
            param.lowerAnimaOneId = index,
            param.lowerAnimeTwoId = index,
            param.lowerAnimeThreeId = index,
            param.lowerAnimeFourId = index,
            param.lowerAnimeFiveId = index,
            param.lowerAnimaSixId = index,
            param.lowerAnimaSevenId = index,
            param.lowerAnimaEightId = index,
            param.loop = loop,
            param.rootFlag = false,
            param.blendType = 0,
            param.animeChangeRate = 0.2f,
            param.animeBlendRate = 0.0f
        };

        return param;
    };

#pragma region Idle
void BaseCharacter_IdleState::Enter()
{
    //アニメーション検索
    int animaIndex = animationCom.lock()->FindAnimation("Idle");

    //下半身だけ再生
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(MakeNormalLowerAnimeParam(animaIndex, true));
}
void BaseCharacter_IdleState::Execute(const float& elapsedTime)
{
    //移動
    if (owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }
    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown() && moveCom.lock()->OnGround())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}
void BaseCharacter_IdleState::Exit()
{
}
#pragma endregion

#pragma region Move
void BaseCharacter_MoveState::Enter()
{
    //歩きアニメーション再生開始
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);

    AnimationCom::PlayLowBodyAnimParam param =
    {
        param.lowerAnimaOneId = animationCom.lock()->FindAnimation("Walk_Forward"),
        param.lowerAnimeTwoId = animationCom.lock()->FindAnimation("Walk_Back"),
        param.lowerAnimeThreeId = animationCom.lock()->FindAnimation("Walk_Right"),
        param.lowerAnimeFourId = animationCom.lock()->FindAnimation("Walk_Left"),
        param.lowerAnimeFiveId = animationCom.lock()->FindAnimation("Walk_RF"),
        param.lowerAnimaSixId = animationCom.lock()->FindAnimation("Walk_LF"),
        param.lowerAnimaSevenId = animationCom.lock()->FindAnimation("Walk_RB"),
        param.lowerAnimaEightId = animationCom.lock()->FindAnimation("Walk_LB"),
        param.loop = true,
        param.rootFlag = false,
        param.blendType = 2,
        param.animeChangeRate = 0.5f,
        param.animeBlendRate = 0.0f
    };

    animationCom.lock()->PlayLowerBodyOnlyAnimation(param);

    //足元の煙エフェクト再生
    GameObjectManager::Instance().Find("smokeeffect")->GetComponent<CPUParticle>()->SetActive(true);
}

void BaseCharacter_MoveState::Execute(const float& elapsedTime)
{
    //移動
    MoveInputVec(owner->GetGameObject());

    //待機
    if (!owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }
    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown() && moveCom.lock()->OnGround())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

void BaseCharacter_MoveState::Exit()
{
    //足元の煙エフェクト停止
    GameObjectManager::Instance().Find("smokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
}

#pragma endregion

#pragma region Jump

void BaseCharacter_JumpState::Enter()
{
    //初期設定
    moveCom.lock()->SetAirForce(12.620);

    //ジャンプ
    JumpInput(owner->GetGameObject(), 1.5f);

    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(MakeNormalLowerAnimeParam(animationCom.lock()->FindAnimation("Jump_begin"), false));
}

void BaseCharacter_JumpState::Execute(const float& elapsedTime)
{
    //ホバリング
    if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
    {
        Hovering(elapsedTime);
    }

    if (!animationCom.lock()->IsPlayAnimation())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMPLOOP);
    }
}
#pragma endregion

#pragma region JumpLoop
void BaseCharacter_JumpLoop::Enter()
{
    //アニメーション検索
    int animaIndex = animationCom.lock()->FindAnimation("Jump_middle");

    //下半身だけ再生
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(MakeNormalLowerAnimeParam(animaIndex, true));
}
void BaseCharacter_JumpLoop::Execute(const float& elapsedTime)
{
    //ホバリング
    if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
    {
        Hovering(elapsedTime);
    }

    if (moveCom.lock()->OnGround())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::LANDING);
    }
}
#pragma endregion

#pragma region Landing
void BaseCharacter_Landing::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(MakeNormalLowerAnimeParam(animationCom.lock()->FindAnimation("Jump_end"), false));
}
void BaseCharacter_Landing::Execute(const float& elapsedTime)
{
    //ホバリング
    if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
    {
        Hovering(elapsedTime);
    }

    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }

    //移動
    if (owner->IsPushLeftStick() && !animationCom.lock()->IsPlayAnimation())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }
}
void BaseCharacter_Landing::Exit()
{
    HoveringTimer = 0.0f;
}
#pragma endregion

#pragma region Death
void BaseCharacter_DeathState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Down"), false);

    //プレイヤー映す
    owner->GetGameObject()->GetComponent<RendererCom>()->SetDissolveThreshold(0);
    //FPS用オブジェクト隠す
    GameObjectManager::Instance().Find("armChild")->GetComponent<RendererCom>()->SetDissolveThreshold(1);

    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        //イベントカメラ
        GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/playerDeath.eventcamera");

        RespawnCom* respawn = GameObjectManager::Instance().Find("respawn")->GetComponent<RespawnCom>().get();
        respawn->SetIsRespawn(true);
    }
}
#pragma endregion

#pragma region Hitscan
void BaseCharacter_HitscanState::Execute(const float& elapsedTime)
{
    auto& ray = owner->GetGameObject()->GetChildFind("rayObj");
    if (ray)
    {
        //視点の向きにレイを飛ばす
        auto& rayCol = ray->GetComponent<RayColliderCom>();
        if (rayCol)
        {
            //有効に
            rayCol->SetEnabled(true);

            DirectX::XMFLOAT3 pos = ray->transform_->GetWorldPosition();

            //自分か判断する
            DirectX::XMFLOAT3 front;
            int playerNetID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetCharaData().GetNetPlayerID();
            if (playerNetID == charaCom.lock()->GetNetCharaData().GetNetPlayerID())
                front = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetWorldFront();
            else
                front = charaCom.lock()->GetFpsCameraDir();

            rayCol->SetStart(pos);
            rayCol->SetEnd(pos + front * rayLength);
        }
    }

    if (!(CharacterInput::MainAttackButton & owner->GetButton()))
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void BaseCharacter_HitscanState::Exit()
{
    //無効に
    auto& ray = owner->GetGameObject()->GetChildFind("rayObj");
    if (ray)
    {
        auto& rayCol = ray->GetComponent<RayColliderCom>();
        if (rayCol)
            rayCol->SetEnabled(false);
    }
}

void BaseCharacter_HitscanState::ImGui()
{
    ImGui::DragFloat("rayLength", &rayLength);
}

#pragma endregion

#pragma region Capsule
void BaseCharacter_CapsuleState::Execute(const float& elapsedTime)
{
    auto& capsule = owner->GetGameObject()->GetChildFind("capsuleObj");
    if (capsule)
    {
        auto& capsuleCol = capsule->GetComponent<CapsuleColliderCom>();
        if (capsuleCol)
        {
            //有効に
            capsuleCol->SetEnabled(true);

            //自分か判断する
            DirectX::XMFLOAT3 front;
            int playerNetID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetCharaData().GetNetPlayerID();
            if (playerNetID == charaCom.lock()->GetNetCharaData().GetNetPlayerID())
                front = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetWorldFront();
            else
                front = charaCom.lock()->GetFpsCameraDir();

            capsuleCol->SetPosition1(DirectX::XMFLOAT3(0, 0, 0));
            capsuleCol->SetPosition2(front * capsuleLength);
        }
    }

    //サブよりメインを優先
    if (CharacterInput::MainAttackButton & owner->GetButtonDown())
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);

    if (!(CharacterInput::SubAttackButton & owner->GetButton()))
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
void BaseCharacter_CapsuleState::Exit()
{
    //無効に
    auto& capsule = owner->GetGameObject()->GetChildFind("capsuleObj");
    if (capsule)
    {
        auto& capsuleCol = capsule->GetComponent<CapsuleColliderCom>();
        if (capsuleCol)
            capsuleCol->SetEnabled(false);
    }
}
void BaseCharacter_CapsuleState::ImGui()
{
    ImGui::DragFloat("capsuleLength", &capsuleLength);
}
#pragma endregion

#pragma region ULT_ATTACK
void Ult_Attack_State::Enter()
{
    obj = owner->GetGameObject()->GetChildFind("UltAttackChild");
    if (!obj)return;

    //レイ設定
    auto& ray = obj->GetComponent<RayColliderCom>();
    DirectX::XMFLOAT3 start = obj->transform_->GetWorldPosition();

    auto& camera = GameObjectManager::Instance().Find("cameraPostPlayer");
    DirectX::XMFLOAT3 front = camera->transform_->GetWorldFront();
    DirectX::XMFLOAT3 end = start + front * 100;

    //エフェクト
    auto& arm = camera->GetChildFind("armChild");
    DirectX::XMFLOAT3 gunPos = {};
    if (arm)
    {
        const auto& model = arm->GetComponent<RendererCom>()->GetModel();
        const auto& node = model->FindNode("gun2");

        gunPos = { node->worldTransform._41,node->worldTransform._42,node->worldTransform._43 };
    }

    //用意したエフェクトオブジェクト起動
    arm->GetChildFind("attackUltMuzzleEff")->GetComponent<GPUParticle>()->Play();

    //音
    owner->GetGameObject()->GetComponent<AudioCom>()->Stop("P_ATTACKULTSHOOT");
    owner->GetGameObject()->GetComponent<AudioCom>()->Play("P_ATTACKULTSHOOT", false, 10);

    //anim
    auto& armAnim = arm->GetComponent<AnimationCom>();
    armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
    armAnim->SetAnimationSeconds(0.3f);

    ray->SetStart(start);
    ray->SetEnd(end);
    ray->SetEnabled(true);
}
void Ult_Attack_State::Execute(const float& elapsedTime)
{
    if (!obj)return;

    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
void Ult_Attack_State::Exit()
{
    auto& ray = obj->GetComponent<RayColliderCom>();
    ray->SetEnabled(false);

    obj.reset();
}
#pragma endregion

void BaseCharacter_ReloadState::Enter()
{
    //アニメーション(腕だけのアニメーション)
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = GameObjectManager::Instance().Find("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_reload"), false);
    }
    else
    {
        auto& animCom = owner->GetGameObject()->GetComponent<AnimationCom>();
        animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Dash"), true);
    }
}

void BaseCharacter_ReloadState::Execute(const float& elapsedTime)
{
    AnimationCom* anima = nullptr;
    bool reloadEnd = false;

    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        //アニメーションが終われば終了フラグを立てる
        auto& arm = GameObjectManager::Instance().Find("armChild");
        anima = arm->GetComponent<AnimationCom>().get();
        reloadEnd = !anima->IsPlayAnimation();
    }
    else
    {
        //リロードアニメーションと同じ時間たてば終了フラグを立てる
        reloadTimer += elapsedTime;
        reloadEnd = (reloadTimer >= reloadTime);
    }

    //終了処理
    if (reloadEnd)
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
        charaCom.lock()->SetMaxBullet();
    }
}

void BaseCharacter_NoneAttack::Enter()
{
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = GameObjectManager::Instance().Find("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        if (!arm->GetComponent<RendererCom>()->GetModel()) return;
        if (arm->GetComponent<AnimationCom>()->IsPlayAnimation()) return;

        auto& stateMachine = owner->GetMoveStateMachine();

        //それぞれのステートによるアニメーション(腕だけのアニメーション)
        PlayStateAnimation(true, stateMachine.GetCurrentState());
    }
    else
    {
        auto& stateMachine = owner->GetMoveStateMachine();
        if (owner->GetGameObject()->GetComponent<AnimationCom>()->IsPlayUpperAnimation()) return;

        //それぞれのステートによるアニメーション(腕だけのアニメーション)
        PlayStateAnimation(false, stateMachine.GetCurrentState());
    }
}

void BaseCharacter_NoneAttack::Execute(const float& elapsedTime)
{
    auto& stateMachine = owner->GetMoveStateMachine();

    //それぞれのステートによるアニメーション(腕だけのアニメーション)
    if (stateMachine.GetCurrentState() != stateMachine.GetOldState())
    {
        PlayStateAnimation(std::string(owner->GetGameObject()->GetName()) == "player", stateMachine.GetCurrentState());
    }
}

void BaseCharacter_NoneAttack::PlayStateAnimation(bool isPlayer, CharacterCom::CHARACTER_MOVE_ACTIONS state)
{
    AnimationCom* animCom = nullptr;
    GameObj arm;
    if (isPlayer)
    {
        arm = GameObjectManager::Instance().Find("armChild");
        animCom = arm->GetComponent<AnimationCom>().get();
    }
    else
    {
        animCom = owner->GetGameObject()->GetComponent<AnimationCom>().get();
    }
    float animeSpeed = 0;

    switch (state)
    {
    case CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE:

        isPlayer ?
            animCom->PlayAnimation(animCom->FindAnimation("FPS_idol"), true) :
            animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Idle"), true);
        break;

    case CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE:

        if (isPlayer)
        {
            animCom->PlayAnimation(animCom->FindAnimation("FPS_walk"), true);

            //アニメーションスピード変更
            animeSpeed = owner->GetGameObject()->GetComponent<MovementCom>()->GetMoveMaxSpeed() - owner->GetGameObject()->GetComponent<MovementCom>()->GetFisrtMoveMaxSpeed();
            if (animeSpeed < 0)animeSpeed = 0;

            arm->GetComponent<RendererCom>()->GetModel()->GetResource()->GetAnimationsEdit()[animCom->FindAnimation("FPS_walk")].animationspeed
                = 1 + animeSpeed * 0.1f;
        }
        else
        {
            if (owner->GetDashFlag())
            {
                animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Dash"), true);
            }
            else
            {
                animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Walk_Forward"), true);
            }
        }

        break;

    case CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP:
        isPlayer ?
            animCom->PlayAnimation(animCom->FindAnimation("FPS_Jump_begin"), false) :
            animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Jump_begin"), false);
        break;

    case CharacterCom::CHARACTER_MOVE_ACTIONS::JUMPLOOP:
        isPlayer ?
            animCom->PlayAnimation(animCom->FindAnimation("FPS_Jump_middle"), false) :
            animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Jump_middle"), false);
        break;

    case CharacterCom::CHARACTER_MOVE_ACTIONS::LANDING:
        isPlayer ?
            animCom->PlayAnimation(animCom->FindAnimation("FPS_Jump_end"), false) :
            animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Jump_end"), false);        break;
        break;

    default:
        break;
    }
}