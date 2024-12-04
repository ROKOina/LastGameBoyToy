#include "JankratCharacterCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "StateMachine\Behaviar\JankratCharacterState.h"
#include "Component\SkillObj\JankratMineCom.h"

void JankratCharacterCom::Start()
{
    //�X�e�[�g�o�^
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<JankratCharacter_MainAtkState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_SKILL, std::make_shared<JankratCharacter_MainSkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK, std::make_shared<JankratCharacter_SubAttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);

    //SE�o�^
}

void JankratCharacterCom::Update(float elapsedTime)
{
    //�X�V
    CharacterCom::Update(elapsedTime);
    
    //�㏈��
    EraseHaveObjects();
}

void JankratCharacterCom::MainAttackDown()
{
    //�e����
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}

void JankratCharacterCom::MainSkill()
{
    //�n���ݒu
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_SKILL);
}

void JankratCharacterCom::SubAttackDown()
{
    //�n���N��
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);
}

void JankratCharacterCom::EraseHaveObjects()
{
    std::vector<GameObj> eraseObjs;
    for (int i = 0; i < haveMine.size(); ++i)
    {
        if (haveMine[i]->GetComponent<JankratMineCom>()->GetExplosionFlag())
        {
            eraseObjs.emplace_back(haveMine[i]);
        }
    }
    for (auto& mine : eraseObjs)
    {
        ReleaseHaveMine(mine);
    }
}
