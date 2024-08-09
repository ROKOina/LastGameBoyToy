#include "BaseCharacterState.h"
#include "Input\Input.h"
#include "BaseCharacterState.h"
#include "Components/ColliderCom.h"
#include "Netwark/Photon/StaticSendDataManager.h"

BaseCharacter_BaseState::BaseCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

void BaseCharacter_IdleState::Enter()
{
    ////歩きアニメーション再生開始
    //animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);

    //AnimationCom::PlayLowBodyAnimParam param =
    //{
    //    param.lowerAnimaOneId = animationCom.lock()->FindAnimation("Idle"),
    //    param.loop = true,
    //    param.rootFlag = false,
    //    param.blendType = 0,
    //    param.animeChangeRate = 0.5f,
    //    param.animeBlendRate = 0.0f

    //};


    //animationCom.lock()->PlayLowerBodyOnlyAnimation(param);
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);

}

void BaseCharacter_IdleState::Execute(const float& elapsedTime)
{
    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //移動
    if(owner->IsPushLeftStick())
    //if (moveVec != 0)
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }
    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

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
        param.loop = true,
        param.rootFlag = false,
        param.blendType = 2,
        param.animeChangeRate = 0.5f,
        param.animeBlendRate = 0.0f

    };

    animationCom.lock()->PlayLowerBodyOnlyAnimation(param);
}

void BaseCharacter_MoveState::Execute(const float& elapsedTime)
{
    MoveInputVec(owner->GetGameObject());

    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //待機
    if(!owner->IsPushLeftStick())
    //if (moveVec == 0)
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }
    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

void BaseCharacter_JumpState::Enter()
{
    //ジャンプ
    if (!moveCom.lock()->OnGround())
        return;

    JumpInput(owner->GetGameObject());
    moveVec = SceneManager::Instance().InputVec();
    moveCom.lock()->SetOnGround(false);

    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump_Enter"), false);
}

void BaseCharacter_JumpState::Execute(const float& elapsedTime)
{
    //空中制御
    DirectX::XMFLOAT3 inputVec = SceneManager::Instance().InputVec();
    moveVec = Mathf::Lerp(moveVec, inputVec, 0.1f);

    if (moveCom.lock()->GetVelocity().y < 0.05f && HoveringTimer < HoveringTime)
    {
        DirectX::XMFLOAT3 verocity = moveCom.lock()->GetVelocity();
        verocity.y = -GRAVITY_NORMAL * elapsedTime;
        moveCom.lock()->SetVelocity(verocity);

        HoveringTimer += elapsedTime;
    }

    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);

    if (moveCom.lock()->OnGround())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }
}

void BaseCharacter_JumpState::Exit()
{
    HoveringTimer = 0.0f;
}

void BaseCharacter_HitscanState::Enter()
{
}

void BaseCharacter_HitscanState::Execute(const float& elapsedTime)
{
    auto& ray = owner->GetGameObject()->GetChildFind("rayObj");
    if (ray)
    {
        //有効に
        ray->SetEnabled(true);

        //視点の向きにレイを飛ばす
        auto& rayCol = ray->GetComponent<RayColliderCom>();
        if (rayCol)
        {
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

            //ヒットを送信
            for (auto& hit : rayCol->OnHitGameObject())
            {
                auto& chara = hit.gameObject.lock()->GetComponent<CharacterCom>();
                if (!chara)continue;
                int hitID = chara->GetNetID();
                StaticSendDataManager::Instance().SetSendDamage(charaCom.lock()->GetNetID(), hitID, 1);
            }
        }
    }

    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void BaseCharacter_HitscanState::Exit()
{
    //無効に
    auto& ray = owner->GetGameObject()->GetChildFind("rayObj");
    if (ray)ray->SetEnabled(false);

}

void BaseCharacter_HitscanState::ImGui()
{
    ImGui::DragFloat("rayLength", &rayLength);
}

void BaseCharacter_NoneAttack::Enter()
{
    ////歩きアニメーション再生開始
    //animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    //animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("Idle"), true, 0.1f);
}
