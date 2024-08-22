#include "InazawaCharacterCom.h"
#include "State\InazawaCharacterState.h"
#include "State\BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "../CameraCom.h"

void InazawaCharacterCom::Start()
{
    //�X�e�[�g�o�^
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<InazawaCharacter_AttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<InazawaCharacter_ESkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));


    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

void InazawaCharacterCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);

    //�W�����v�_�b�V������
    auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    if (moveCom->OnGround())
    {
        isDashJump = false;
        airTimer = 0.0f;
    }
    else
    {
        airTimer += elapsedTime;
    }

    //�X�L��E�N�[���^�C��
    if (useSkillE)
    {
        eCoolTimer += elapsedTime;
        if (eCoolTime < eCoolTimer)
        {
            eCoolTimer = 0;
            useSkillE = false;
        }
    }
}

void InazawaCharacterCom::OnGUI()
{
    CharacterCom::OnGUI();

    if (ImGui::TreeNode("InaChara"))
    {
        bool p = useSkillE;
        ImGui::Checkbox("useSkillE", &p);
        ImGui::DragFloat("eCoolTimer", &eCoolTimer);
        ImGui::TreePop();
    }
}

void InazawaCharacterCom::MainAttack()
{
    if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::SUB_SKILL)
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}

void InazawaCharacterCom::SubSkill()
{
    if (!useSkillE)
    {
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL);
        useSkillE = true;
    }
}

void InazawaCharacterCom::SpaceSkill()
{
    auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    if (!moveCom->OnGround() && !isDashJump && airTimer > 0.1f)
    {
        GamePad gamePad = Input::Instance().GetGamePad();

        //���͒l�擾
        DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

        //����
        DirectX::XMFLOAT3 v = moveVec * 50.0f;
        moveCom->AddNonMaxSpeedForce(v);

        ////���񏈗�
        //GetGameObject()->transform_->Turn(moveVec, 1);

        moveCom->ZeroVelocity();
        JumpInput(GetGameObject());
        isDashJump = true;
    }
}
