#include "InazawaCharacterCom.h"
#include "State\TestCharacterState.h"

void InazawaCharacterCom::Start()
{
    //�X�e�[�g�o�^
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<TestCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<TestCharacter_JumpState>(this));

    stateMachine.ChangeState(CHARACTER_ACTIONS::MOVE);
}
