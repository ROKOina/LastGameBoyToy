#include "FarahCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "Component\Renderer\RendererCom.h"
#include "Math\Mathf.h"

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
    ImGui::DragFloat("frontvecpower", &frontvecpower);
}

//�E�N���b�N�P����������
void FarahCom::SubAttackDown()
{
    // �X�e�[�g�ύX
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::JUMP);

    // movecomp�擾
    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    // �O�����x�N�g�����擾�i���X�e�B�b�N���͂ɂ��������A���[���h�O�������j
    DirectX::XMVECTOR frontVec = IsPushLeftStick()
        ? DirectX::XMLoadFloat3(&SceneManager::Instance().InputVec(GetGameObject()))
        : DirectX::XMLoadFloat3(&GetGameObject()->transform_->GetWorldFront());
    frontVec = DirectX::XMVector3Normalize(frontVec); // ���K��

    // �_�b�V���͂��v�Z
    DirectX::XMVECTOR dashForce = DirectX::XMVectorScale(frontVec, dashProgress * frontvecpower);
    DirectX::XMFLOAT3 dashForceFloat3;
    DirectX::XMStoreFloat3(&dashForceFloat3, dashForce);

    DirectX::XMFLOAT3 liftForce =
    {
        GetGameObject()->transform_->GetWorldPosition().x,
        dashProgress, // �㏸�͂��X�V
        GetGameObject()->transform_->GetWorldPosition().z
    };

    // �͂��ړ��R���|�[�l���g�ɉ�����
    moveCom->AddForce(liftForce); // �󒆂̋���
    moveCom->AddNonMaxSpeedForce({ dashForceFloat3.x,0.0f,dashForceFloat3.z }); // �O�����̈ړ�
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