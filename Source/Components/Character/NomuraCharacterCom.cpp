#include "NomuraCharacterCom.h"
#include "State/BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "State/NomuraCharacterState.h"

void NomuraCharacterCom::Start()
{
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<NomuraCharacter_AttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK, std::make_shared<NomuraCharacter_SubAttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<NomuraCharacter_ESkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::RELOAD, std::make_shared<NomuraCharacter_ReloadState>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::MOVE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

void NomuraCharacterCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);


    FireInterval(elapsedTime);

    BulletManagement();
}

void NomuraCharacterCom::OnGUI()
{
    CharacterCom::OnGUI();

    if (ImGui::TreeNode("NomChara"))
    {
        ImGui::Checkbox("launchPermission", &launchPermission);
        ImGui::Checkbox("remainingBullets", &remainingBullets);
        ImGui::DragInt("nowMagazine", &nowMagazine);
        ImGui::DragFloat("firingIntervalTimer", &firingIntervalTimer);
        ImGui::DragFloat("firingInterval", &firingInterval);
        ImGui::TreePop();
    }
}



void NomuraCharacterCom::MainAttack()
{
    if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::SUB_SKILL && launchPermission&& remainingBullets)
    {
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
        launchPermission = false;
        nowMagazine -= 1;

        firingIntervalTimer = firingInterval;
    }
}

void NomuraCharacterCom::SubAttack()
{
    if (nowMagazine > 0)
    {
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);
    }
}

void NomuraCharacterCom::SubSkill()
{
    /*auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    DirectX::XMVECTOR MoveVec = DirectX::XMLoadFloat3(&moveVec);

    if (moveVec != 0)
    {
        MoveVec = DirectX::XMVectorScale(MoveVec, 50.0f);

        DirectX::XMStoreFloat3(&moveVec, MoveVec);

        moveCom->AddNonMaxSpeedForce(moveVec);
    }
    else
    {
        MoveVec = DirectX::XMLoadFloat3(&GetGameObject()->transform_->GetWorldFront());

        MoveVec = DirectX::XMVectorScale(MoveVec, 50.0f);

        DirectX::XMStoreFloat3(&moveVec, MoveVec);

        moveCom->AddNonMaxSpeedForce(moveVec);
    }

    Reload();*/
}

void NomuraCharacterCom::UltSkill()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::RELOAD);
}

//void NomuraCharacterCom::SpaceSkill()
//{
//
//}


void NomuraCharacterCom::FireInterval(float elapsedTime)
{
    firingIntervalTimer -= elapsedTime;

    if (firingIntervalTimer < 0.0f)
    {
        firingIntervalTimer = 0.0f;

        launchPermission = true;
    }

}

void NomuraCharacterCom::Reload()
{
    nowMagazine = maxBullets;
    remainingBullets = true;
}

void NomuraCharacterCom::BulletManagement()
{
    if (nowMagazine == 0)
    {
        remainingBullets = false;
    }
}

void NomuraCharacterCom::ReloadManagement()
{
    
}