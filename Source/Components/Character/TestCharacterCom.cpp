#include "TestCharacterCom.h"
#include "../MovementCom.h"
#include "../TransformCom.h"
#include "../RendererCom.h"
#include "Input\Input.h"
#include "State\TestCharacterState.h"
#include "State\BaseCharacterState.h"

void TestCharacterCom::Start()
{
    //ÉXÉeÅ[Égìoò^
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<TestCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));

    stateMachine.ChangeState(CHARACTER_ACTIONS::MOVE);
}

void TestCharacterCom::MainAttack()
{
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");

    DirectX::XMFLOAT3 firePos = GetGameObject()->transform_->GetWorldPosition();
    firePos.y = 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    moveCom->SetGravity(0.0f);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(GetGameObject()->transform_->GetWorldFront() * 30.0f);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>(static_cast<int>(SHADERMODE::BLACK), static_cast<int>(BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");
}


