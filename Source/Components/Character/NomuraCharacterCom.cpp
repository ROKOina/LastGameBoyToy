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





}

void NomuraCharacterCom::OnGUI()
{
    CharacterCom::OnGUI();

    if (ImGui::TreeNode("NomChara"))
    {
        ImGui::TreePop();
    }
}



void NomuraCharacterCom::MainAttack()
{
    if (stateMachine.GetCurrentState() != CHARACTER_ACTIONS::DASH)
        stateMachine.ChangeState(CHARACTER_ACTIONS::ATTACK);
}

void NomuraCharacterCom::SubSkill()
{

}

//void NomuraCharacterCom::SpaceSkill()
//{
//
//}