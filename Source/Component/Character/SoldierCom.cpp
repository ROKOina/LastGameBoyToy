#include "SoldierCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "StateMachine\Behaviar\SolderState.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Particle\GPUParticle.h"

//������
void SoldierCom::Start()
{
    // �X�e�[�g�o�^ (�ړ�)
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<BaseCharacter_JumpLoop>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::LANDING, std::make_shared<BaseCharacter_Landing>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    // �X�e�[�g�o�^ (�U��)
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<Solder_MainAttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<Solder_ESkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::ULT, std::make_shared<Solder_UltState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::RELOAD, std::make_shared<BaseCharacter_ReloadState>(this));

    // �����X�e�[�g�ݒ�
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

// �X�V����
void SoldierCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);

    //�q�b�g�X�L�����������������̏���
    HitObject();
}

// �E�N���b�N�P����������
void SoldierCom::SubAttackDown()
{
}

// E�X�L��
void SoldierCom::SubSkill()
{
    if (!UseUlt())
    {
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL);
    }
    else
    {
        ResetSkillCoolTimer(SkillCoolID::E);
    }
}

// ���C���U��
void SoldierCom::MainAttackDown()
{
    //�X�L���������̓��^�[��
    if (attackStateMachine.GetCurrentState() == CHARACTER_ATTACK_ACTIONS::SUB_SKILL)return;

    //�A�^�b�N
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
    attackInputSave = false;
}

// ULT
void SoldierCom::UltSkill()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::ULT);
}

//�����[�h
void SoldierCom::Reload()
{
    if (currentBulletNum < maxBulletNum)
    {
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::RELOAD);
    }
}

// GUI
void SoldierCom::OnGUI()
{
    CharacterCom::OnGUI();
}

//�q�b�g�X�L�����������������̏���
void SoldierCom::HitObject()
{
    //�q�b�g�X�L�������������
    if (attackray.lock())
    {
        auto& rayCol = attackray.lock()->GetComponent<Collider>();
        if (rayCol)
        {
            for (auto& obj : rayCol->OnHitGameObject())
            {
                //�q�b�g�G�t�F�N�g����
                std::shared_ptr<GameObject> hiteffectobject = GameObjectManager::Instance().Create();
                hiteffectobject->transform_->SetWorldPosition(obj.hitPos);
                hiteffectobject->SetName("HitEffect");
                std::shared_ptr<GPUParticle>Chiteffct = hiteffectobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/hanabi.gpuparticle", 1000);
                Chiteffct->Play();
                std::shared_ptr<CPUParticle>Ghiteffct = hiteffectobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/hitsmokeeffect.cpuparticle", 100);
                Ghiteffct->SetActive(true);
            }
        }
    }
}