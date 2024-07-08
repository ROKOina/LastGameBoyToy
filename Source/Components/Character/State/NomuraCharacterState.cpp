#include "NomuraCharacterState.h"
#include "Input\Input.h"

#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\Character\BulletCom.h"
#include "GameSource/Scene/SceneManager.h"
#include "BaseCharacterState.h"


// �}�N��
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) testCharaCom.lock()->GetStateMachine().ChangeState(State);

void BuletFire(std::shared_ptr<GameObject> objPoint, float arrowSpeed = 40, float power = 1)
{
    //�e�ۃI�u�W�F�N�g�𐶐�///////

    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //�e����
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 10 - power * 9;
    moveCom->SetGravity(-gravity);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(objPoint->transform_->GetWorldFront() * (20.0f + arrowSpeed * power));

    //�e
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>();
    bulletCom->SetAliveTime(2.0f);
}



NomuraCharacter_BaseState::NomuraCharacter_BaseState(CharacterCom* owner) :State(owner)
{
    //�����ݒ�
    testCharaCom = GetComp(NomuraCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region Attack

void NomuraCharacter_AttackState::Enter()
{

}

void NomuraCharacter_AttackState::Execute(const float& elapsedTime)
{
    

    MoveInputVec(owner->GetGameObject(), 0.5f);

    if (moveCom.lock()->OnGround())
        JumpInput(owner->GetGameObject());

    //�U���З�
    attackPower += elapsedTime;
    if (attackPower > maxAttackPower) {
        attackPower = maxAttackPower;
    }
    
    //�U���I���������U������
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //�U������
        BuletFire(owner->GetGameObject(), 30.0f, attackPower);

        ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);

    }
}

void NomuraCharacter_AttackState::ImGui()
{
  
}

#pragma endregion

#pragma region ESkill

void NomuraCharacter_ESkillState::Enter()
{

}

void NomuraCharacter_ESkillState::Execute(const float& elapsedTime)
{
    




}

void NomuraCharacter_ESkillState::ImGui()
{

}

#pragma endregion