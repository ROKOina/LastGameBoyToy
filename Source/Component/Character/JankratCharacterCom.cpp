#include "JankratCharacterCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "StateMachine\Behaviar\JankratCharacterState.h"
#include "Component\SkillObj\JankratMineCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Particle\CPUParticle.h"

void JankratCharacterCom::Start()
{
    //�X�e�[�g�o�^
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<BaseCharacter_JumpLoop>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::LANDING, std::make_shared<BaseCharacter_Landing>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<JankratCharacter_MainAtkState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_SKILL, std::make_shared<JankratCharacter_MainSkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK, std::make_shared<JankratCharacter_SubAttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::ULT, std::make_shared<JankratCharacter_UltState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);

    //SE�o�^
}

void JankratCharacterCom::Update(float elapsedTime)
{
    //�e�̑łԊu�ƃ}�[���t���b�V��
    ShotSecond();

    //�X�V
    CharacterCom::Update(elapsedTime);

    //fps�p�̘r�A�j���[�V����
    FPSArmAnimation();

    //�㏈��
    EraseHaveObjects();
}

void JankratCharacterCom::MainAttackDown()
{
    //�X�L���������̓��^�[��
    if (attackStateMachine.GetCurrentState() == CHARACTER_ATTACK_ACTIONS::SUB_SKILL)return;
    if (attackStateMachine.GetCurrentState() == CHARACTER_ATTACK_ACTIONS::ULT)return;

    //�e����
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
    attackInputSave = false;
}

//ESKILL
void JankratCharacterCom::SubSkill()
{
    if (!UseUlt())
    {
        //�n���ݒu
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_SKILL);
    }
    else
    {
        ResetSkillCoolTimer(SkillCoolID::E);
    }
}

//ult
void JankratCharacterCom::UltSkill()
{
    //ult�X�e�[�g�ɑJ��
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::ULT);
}

//�N���X�e�[�g�ɑJ��
void JankratCharacterCom::SubAttackDown()
{
    //�n���N��
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);
}

//�S�ẴW�����N���̋N�����u���폜
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

//�e�̑łԊu�ƃ}�[���t���b�V��
void JankratCharacterCom::ShotSecond()
{
    const auto& arm = GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
    const auto& particle = arm->GetChildFind("muzzleflash");
    DirectX::XMFLOAT3 pos = {};
    if (arm->GetComponent<AnimationCom>()->IsEventCallingNodePos("MUZZLEFLASH", "gun2", pos))
    {
        particle->transform_->SetWorldPosition(pos);
        particle->GetComponent<CPUParticle>()->SetActive(true);
    }
    else
    {
        particle->GetComponent<CPUParticle>()->SetActive(false);
    }
}