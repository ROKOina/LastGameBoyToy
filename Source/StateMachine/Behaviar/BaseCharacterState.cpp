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
        DirectX::XMFLOAT3 force = {
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

#pragma region Idle

void BaseCharacter_IdleState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);

    ////FPS用
    //auto& camera = owner->GetGameObject()->GetChildFind("cameraPostPlayer");
    //if (camera)
    //{
    //    auto& arm = camera->GetChildFind("armChild");
    //    auto& armAnim = arm->GetComponent<AnimationCom>();

    //    armAnim->PlayAnimation(armAnim->FindAnimation("FPS_idol"), true);
    //}
}

void BaseCharacter_IdleState::Execute(const float& elapsedTime)
{
    //移動
    if (owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }
    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
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
    //animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("Single_Shot"), false, 0.3f);
    GameObjectManager::Instance().Find("smokeeffect")->GetComponent<CPUParticle>()->SetActive(true);

    /*animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Walk_Forward"), true);*/
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
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

void BaseCharacter_MoveState::Exit()
{
    GameObjectManager::Instance().Find("smokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
}

#pragma endregion

#pragma region Jump

void BaseCharacter_JumpState::Enter()
{
    //ジャンプ
    JumpInput(owner->GetGameObject());

    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump_begin"), false);

    //アニメーション
    auto& arm = GameObjectManager::Instance().Find("armChild");
    auto& armAnim = arm->GetComponent<AnimationCom>();
    armAnim->PlayAnimation(armAnim->FindAnimation("FPS_Jump_begin"), false);
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
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump_middle"), false);

    //アニメーション
    auto& arm = GameObjectManager::Instance().Find("armChild");
    auto& armAnim = arm->GetComponent<AnimationCom>();
    if (!armAnim->IsPlayAnimation())
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_Jump_middle"), true);
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
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump_end"), false, false, 1.0f);

    //アニメーション
    auto& arm = GameObjectManager::Instance().Find("armChild");
    auto& armAnim = arm->GetComponent<AnimationCom>();
    armAnim->PlayAnimation(armAnim->FindAnimation("FPS_Jump_end"), false);
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

    //イベントカメラ
    GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();
    EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/playerDeath.eventcamera");

    //遷移
    SceneManager::Instance().ChangeSceneDelay(new SceneTitle, 3);
}

void BaseCharacter_DeathState::Execute(const float& elapsedTime)
{
}

void BaseCharacter_DeathState::Exit()
{
}

#pragma endregion

#pragma region Hitscan

void BaseCharacter_HitscanState::Enter()
{
}

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
            int playerNetID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetID();
            if (playerNetID == charaCom.lock()->GetNetID())
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

void BaseCharacter_CapsuleState::Enter()
{
}

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
            int playerNetID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetID();
            if (playerNetID == charaCom.lock()->GetNetID())
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

#pragma region StanBall

void BaseCharacter_StanBallState::Enter()
{
    BulletCreate::DamageFire(owner->GetGameObject(), speed, power);
    //BulletCreate::StanFire(owner->GetGameObject(), speed, power);
}

void BaseCharacter_StanBallState::Execute(const float& elapsedTime)
{
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void BaseCharacter_StanBallState::Exit()
{
}

void BaseCharacter_StanBallState::ImGui()
{
    ImGui::DragFloat("speed", &speed);
    ImGui::DragFloat("power", &power);
}

#pragma endregion

#pragma region KnockbackBall

void BaseCharacter_KnockbackBallState::Enter()
{
    BulletCreate::KnockbackFire(owner->GetGameObject(), speed, power);
}

void BaseCharacter_KnockbackBallState::Execute(const float& elapsedTime)
{
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void BaseCharacter_KnockbackBallState::Exit()
{
}

void BaseCharacter_KnockbackBallState::ImGui()
{
    ImGui::DragFloat("speed", &speed);
    ImGui::DragFloat("power", &power);
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

void Ult_Attack_State::ImGui()
{
}

#pragma endregion

void BaseCharacter_NoneAttack::Enter()
{
    ////歩きアニメーション再生開始
    //animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    //animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("Idle"), true, 0.1f);
}