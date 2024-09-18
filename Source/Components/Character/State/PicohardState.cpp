#include "PicohardState.h"
#include "Components/ColliderCom.h"
#include "Components/RendererCom.h"

Picohard_BaseState::Picohard_BaseState(CharacterCom* owner) : State(owner)
{    
    //初期設定
    charaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}


void Picohard_LeftClick::Enter()
{
    auto& hammerZikuX = owner->GetGameObject()->GetChildFind("hammerZikuX");
    auto& hammerZikuY = hammerZikuX->GetChildFind("hammerZikuY");

    hammerZikuY->transform_->SetEulerRotation({0, rangeAngle, 0});
    angle = rangeAngle;

    auto& hammer = hammerZikuY->GetChildFind("hammer");
    hammer->GetComponent<Collider>()->SetEnabled(true);
}

void Picohard_LeftClick::Execute(const float& elapsedTime)
{
    auto& hammerZikuX = owner->GetGameObject()->GetChildFind("hammerZikuX");
    float x = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetEulerRotation().x;
    hammerZikuX->transform_->SetEulerRotation({ x,0,0 });

    auto& hammerZikuY = hammerZikuX->GetChildFind("hammerZikuY");

    angle -= elapsedTime * speed;
    if (angle < -rangeAngle)angle = rangeAngle;

    hammerZikuY->transform_->SetEulerRotation({ 0, angle, 0 });


    //サブ（盾）を優先
    if (CharacterInput::SubAttackButton & owner->GetButtonDown())
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);


    if (!(CharacterInput::MainAttackButton & owner->GetButton()))
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void Picohard_LeftClick::Exit()
{
    auto& hammerZikuX = owner->GetGameObject()->GetChildFind("hammerZikuX");
    auto& hammerZikuY = hammerZikuX->GetChildFind("hammerZikuY");
    hammerZikuY->transform_->SetEulerRotation({ 0, 0, 0 });

    auto& hammer = hammerZikuY->GetChildFind("hammer");
    hammer->GetComponent<Collider>()->SetEnabled(false);
}



void Picohard_RightClick::Enter()
{
    auto& sieldZiku = owner->GetGameObject()->GetChildFind("sieldZikuX");
    auto& sield = sieldZiku->GetChildFind("sield");

    sield->GetComponent<Collider>()->SetEnabled(true);
    sield->GetComponent<RendererCom>()->SetEnabled(true);
}

void Picohard_RightClick::Execute(const float& elapsedTime)
{
    auto& sieldZiku = owner->GetGameObject()->GetChildFind("sieldZikuX");
    float x = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetEulerRotation().x;
    sieldZiku->transform_->SetEulerRotation({ x,0,0 });

    if (!(CharacterInput::SubAttackButton & owner->GetButton()))
    {
        auto& sield = sieldZiku->GetChildFind("sield");
        sield->GetComponent<Collider>()->SetEnabled(false);
        sield->GetComponent<RendererCom>()->SetEnabled(false);

        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}

void Picohard_RightClick::Exit()
{
}
