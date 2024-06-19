#include "InazawaCharacterCom.h"
#include "State\InazawaCharacterState.h"
#include "State\BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "../CameraCom.h"

void InazawaCharacterCom::Start()
{
    //�X�e�[�g�o�^
    stateMachine.AddState(CHARACTER_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::ATTACK, std::make_shared<InazawaCharacter_AttackState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::DASH, std::make_shared<InazawaCharacter_ESkillState>(this));

    stateMachine.ChangeState(CHARACTER_ACTIONS::MOVE);
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
    if (stateMachine.GetCurrentState() != CHARACTER_ACTIONS::DASH)
        stateMachine.ChangeState(CHARACTER_ACTIONS::ATTACK);
}

void InazawaCharacterCom::SubSkill()
{
    if (!useSkillE)
    {
        stateMachine.ChangeState(CHARACTER_ACTIONS::DASH);
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

        //���񏈗�
        GetGameObject()->transform_->Turn(moveVec, 1);

        moveCom->ZeroVelocity();
        JumpInput(GetGameObject());
        isDashJump = true;
    }
}
