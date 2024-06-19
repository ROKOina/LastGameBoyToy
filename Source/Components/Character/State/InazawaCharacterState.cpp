#include "InazawaCharacterState.h"
#include "Input\Input.h"

#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\Character\BulletCom.h"

// マクロ
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) testCharaCom.lock()->GetStateMachine().ChangeState(State);

InazawaCharacter_BaseState::InazawaCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    testCharaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region Attack

void InazawaCharacter_AttackState::Enter()
{
    
}

void InazawaCharacter_AttackState::Execute(const float& elapsedTime)
{

}

#pragma endregion 

void InazawaCharacter_AttackState::Fire()
{
    //弾丸オブジェクトを生成///////

    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");

    DirectX::XMFLOAT3 firePos = owner->GetGameObject()->transform_->GetWorldPosition();
    firePos.y = 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADERMODE::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //弾発射
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    moveCom->SetGravity(0.0f);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(owner->GetGameObject()->transform_->GetWorldFront() * 30.0f);

    //弾
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>();
    bulletCom->SetAliveTime(2.0f);
}
