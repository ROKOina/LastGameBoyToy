#include "UenoCharacterCom.h"
#include "State\BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "Components/Character/State/UenoCharacterState.h"
#include "../CameraCom.h"
#include "Components/CPUParticle.h"
#include "Components\RendererCom.h"

//������
void UenoCharacterCom::Start()
{
    //�X�e�[�g�o�^
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<UenoCharacterState_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<UenoCharacterState_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<UenoCharacterState_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<UenoCharacterState_JumpLoopState>(this));

    //�U���X�e�[�g
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_SKILL, std::make_shared<UenoCharacterState_ShiftSkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK, std::make_shared<BaseCharacter_HitscanState>(this), AttackType::LAZER);
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this), AttackType::NON);

    //�����o�^
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

//�X�V����
void UenoCharacterCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);
}

//imgui
void UenoCharacterCom::OnGUI()
{
    CharacterCom::OnGUI();
}

//�V�t�g�X�L��(�E�B���X�g���Ȃ̂ŃW�����v�p�b�N)

//���N���b�N
void UenoCharacterCom::MainAttackPushing()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);
}