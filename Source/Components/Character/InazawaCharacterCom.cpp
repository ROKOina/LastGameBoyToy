#include "InazawaCharacterCom.h"
#include "State\InazawaCharacterState.h"
#include "State\BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "../CameraCom.h"

void InazawaCharacterCom::Start()
{
    //ステート登録
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

    //ジャンプダッシュ処理
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

    //スキルEクールタイム
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

        //入力値取得
        DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

        //歩く
        DirectX::XMFLOAT3 v = moveVec * 50.0f;
        moveCom->AddNonMaxSpeedForce(v);

        ////旋回処理
        //GetGameObject()->transform_->Turn(moveVec, 1);

        moveCom->ZeroVelocity();
        JumpInput(GetGameObject());
        isDashJump = true;
    }
}
