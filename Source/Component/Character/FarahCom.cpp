#include "FarahCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "Component\Renderer\RendererCom.h"
#include "Math\Mathf.h"
#include "SystemStruct\TimeManager.h"
#include "StateMachine\Behaviar\FarahState.h"

//������
void FarahCom::Start()
{
    //�X�e�[�g�o�^(�ړ��֌W)
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<BaseCharacter_JumpLoop>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::LANDING, std::make_shared<BaseCharacter_Landing>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    //�X�e�[�g�o�^(�U���֌W)
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<Farah_MainAttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<Farah_ESkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::ULT, std::make_shared<Farah_UltState>(this));

    //�����X�e�[�g
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

//�X�V����
void FarahCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);

    //fps�p�̘r�A�j���[�V����
    FPSArmAnimation();

    // �N�[���^�C���X�V
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer = (std::max)(0.0f, cooldownTimer - elapsedTime);
    }

    //�u�[�X�g����
    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    if (moveCom->OnGround())
    {
        boostflag = false;
    }
    else
    {
        boostflag = true;
    }

    //ult�X�V
    UltUpdate(elapsedTime);
}

//gui
void FarahCom::OnGUI()
{
    ImGui::DragFloat("cooldownTimer", &cooldownTimer);
    CharacterCom::OnGUI();
}

//�E�N���b�N�P����������
void FarahCom::SubAttackDown()
{
    //������
    GetGameObject()->GetComponent<MovementCom>()->SetAirForce(12.620f);

    //�X�e�[�g�ύX
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::JUMP);

    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    //������
    moveCom->SetVelocity({ moveCom->GetVelocity().x,0.0f,moveCom->GetVelocity().z });

    //��u�̔��
    DirectX::XMFLOAT3 power = {
        0.0f,
        Mathf::Lerp(0.0f,13.0f,0.8f),
        0.0f
    };

    // �͂��ړ��R���|�[�l���g�ɉ�����
    moveCom->AddForce(power);

    // �N�[���^�C���ݒ�i0.5�b�j
    cooldownTimer = 0.5f;

    //�_�b�V���Q�[�W����
    dashGauge += 5.0f;
}

//�X�y�[�X�X�L��������
void FarahCom::SpaceSkillPushing(float elapsedTime)
{
    // �N�[���^�C�����͏����𖳌���
    if (cooldownTimer > 0.0f)
    {
        return; // �N�[���_�E�����Ȃ̂ŉ������Ȃ�
    }

    //�㏸
    if (dashGauge > 0.01f)
    {
        if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::JUMPLOOP)
        {
            //�Q�[�W���炷
            dashGauge -= dashgaugemin * elapsedTime;
            GetGameObject()->GetComponent<MovementCom>()->Rising(elapsedTime);
        }
    }
}

//E�X�L��
void FarahCom::SubSkill()
{
    if (!UseUlt())
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL);
    else
        ResetSkillCoolTimer(SkillCoolID::E);
}

//���C���̍U��
void FarahCom::MainAttackDown()
{
    //�e�۔���
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}

//ULT
void FarahCom::UltSkill()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::ULT);
}

//�E���g�X�V
void FarahCom::UltUpdate(float elapsedTime)
{
    //ult�g�p��
    if (UseUlt())
    {
        ulttimer += elapsedTime;

        const auto& move = GetGameObject()->GetComponent<MovementCom>();

        //���Ԃ�ult�����X�e�[�^�X�����ɖ߂�
        if (ulttimer > 15.0f)
        {
            dashgaugemin = 4.0f;
            move->SetMoveAcceleration(3.0f);
            FinishUlt();
            ulttimer = 0.0f;
        }
    }
}