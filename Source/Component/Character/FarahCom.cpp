#include "FarahCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "Component\Renderer\RendererCom.h"
#include "Math\Mathf.h"
#include "SystemStruct\TimeManager.h"

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
}

//gui
void FarahCom::OnGUI()
{
    CharacterCom::OnGUI();
}

//�E�N���b�N�P����������
void FarahCom::SubAttackDown()
{
    // �X�e�[�g�ύX
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::JUMP);

    // �ړ��R���|�[�l���g�擾
    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    // ���݂̎��Ԃ��擾
    float currentTime = TimeManager::Instance().GetElapsedTime();

    // ���������̃_�b�V���͂Ə㏸�͂ɑ΂��錸���ݒ�
    static constexpr float DASH_POWER_BASE = 75.0f;    // �����_�b�V����
    static constexpr float DASH_DURATION = 5.0f;      // �_�b�V���͂��������鎞��
    static constexpr float LIFT_FORCE_BASE = 3.5f;    // �����㏸��
    static constexpr float LIFT_DURATION = 1.0f;      // �㏸�͂��������鎞��
    static constexpr float GRAVITY = 9.8f;            // �d�͉����x

    // ���Ԍo�߂ɉ������_�b�V���͂̌����v�Z
    float dashPower = DASH_POWER_BASE * (1.0f - (currentTime / DASH_DURATION));
    dashPower = (std::max)(dashPower, 0.0f); // �͂����ɂȂ�Ȃ��悤����

    // �O�����x�N�g���i���͕����܂��̓��[���h�O�����j
    DirectX::XMVECTOR frontVec = IsPushLeftStick()
        ? DirectX::XMLoadFloat3(&SceneManager::Instance().InputVec(GetGameObject()))
        : DirectX::XMLoadFloat3(&GetGameObject()->transform_->GetWorldFront());
    frontVec = DirectX::XMVector3Normalize(frontVec); // ���K��

    // ������̃_�b�V���͂��v�Z
    DirectX::XMVECTOR dashForce = DirectX::XMVectorScale(frontVec, dashPower);
    DirectX::XMFLOAT3 dashForceFloat3;
    DirectX::XMStoreFloat3(&dashForceFloat3, dashForce);

    // ���Ԍo�߂ɉ������㏸�͂̌����v�Z
    float verticalForce = LIFT_FORCE_BASE - (GRAVITY * currentTime);
    verticalForce = (std::max)(verticalForce, 0.0f); // �͂����ɂȂ�Ȃ��悤����

    // �ŏI�I�ȗ͂��v�Z
    DirectX::XMFLOAT3 liftForce = {
        0.0f, // X�����͖���
        verticalForce, // �㏸��
        0.0f  // Z�����͖���
    };

    // �͂��ړ��R���|�[�l���g�ɉ�����
    moveCom->AddForce(liftForce); // �㏸��
    moveCom->AddNonMaxSpeedForce({ dashForceFloat3.x, 0.0f, dashForceFloat3.z }); // �����ړ�
}

//�X�y�[�X�X�L��������
void FarahCom::SpaceSkillPushing(float elapsedTime)
{
    //�㏸
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::JUMPLOOP)
    {
        GetGameObject()->GetComponent<MovementCom>()->Rising(elapsedTime);
    }
}

//E�X�L��
void FarahCom::SubSkill()
{
    //�X�e�[�g�ύX
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::JUMP);

    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    //��u�̔��
    DirectX::XMFLOAT3 power = {
        0.0f,
        Mathf::Lerp(0.0f,10.0f,0.8f),
        0.0f
    };

    // �͂��ړ��R���|�[�l���g�ɉ�����
    moveCom->AddForce(power);
}

static float AH = 0;
//fps�p�̘r�A�j���[�V����
void FarahCom::FPSArmAnimation()
{
    if (std::string(GetGameObject()->GetName()) != "player")return;
    auto& arm = GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
    auto& armAnim = arm->GetComponent<AnimationCom>();

    //�ҋ@
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::IDLE)
    {
        if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_idol"))return;

        if (armAnim->GetCurrentAnimationIndex() != armAnim->FindAnimation("FPS_shoot"))
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_idol"), true);
    }

    //�ړ�
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::MOVE)
    {
        if (armAnim->GetCurrentAnimationIndex() != armAnim->FindAnimation("FPS_walk"))
        {
            if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_shoot"))
            {
                if (armAnim->IsEventCalling("attackEnd"))
                    armAnim->PlayAnimation(armAnim->FindAnimation("FPS_walk"), true);
            }
            else
                armAnim->PlayAnimation(armAnim->FindAnimation("FPS_walk"), true);
        }
    }

    //�A�j���[�V�����X�s�[�h�ύX
    float fmax = GetGameObject()->GetComponent<MovementCom>()->GetFisrtMoveMaxSpeed();
    float max = GetGameObject()->GetComponent<MovementCom>()->GetMoveMaxSpeed();

    float v = max - fmax;
    AH = max;
    if (v < 0)v = 0;

    arm->GetComponent<RendererCom>()->GetModel()->GetResource()->GetAnimationsEdit()[armAnim->FindAnimation("FPS_walk")].animationspeed = 1 + v * 0.1f;
}