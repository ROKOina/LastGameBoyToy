#include "NomuraCharacterCom.h"
#include "State/BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "State/NomuraCharacterState.h"

void NomuraCharacterCom::Start()
{
    stateMachine.AddState(CHARACTER_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::ATTACK, std::make_shared<NomuraCharacter_AttackState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::DASH, std::make_shared<NomuraCharacter_ESkillState>(this));
    
    stateMachine.ChangeState(CHARACTER_ACTIONS::MOVE);

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
    if (stateMachine.GetCurrentState() != CHARACTER_ACTIONS::DASH&& launchPermission&& remainingBullets)
    {
        stateMachine.ChangeState(CHARACTER_ACTIONS::ATTACK);
        launchPermission = false;
        nowMagazine -= 1;

        firingIntervalTimer = firingInterval;
    }
}

void NomuraCharacterCom::SubSkill()
{
    auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    DirectX::XMVECTOR MoveVec = DirectX::XMLoadFloat3(&moveVec);

    MoveVec = DirectX::XMVectorScale(MoveVec, 50.0f);

    DirectX::XMStoreFloat3(&moveVec, MoveVec);

    moveCom->AddNonMaxSpeedForce(moveVec);

    //moveCom->AddForce(moveVec);
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