#include "CharacterCom.h"
#include "../MovementCom.h"
#include "../CameraCom.h"
#include "../TransformCom.h"

#include "Input\Input.h"
#include "../../GameSource/Math/Mathf.h"

void CharacterCom::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    stateMachine.Update(elapsedTime);


    if (CharacterInput::MainAttackButton & gamePad.GetButtonDown())
    {
        MainAttack();
    }

    if (CharacterInput::MainSkillButton & gamePad.GetButtonDown())
    {
        MainSkill();
    }
    if (CharacterInput::SubSkillButton & gamePad.GetButtonDown())
    {
        SubSkill();
    }
}

void CharacterCom::OnGUI()
{
    std::string stateNames[(int)CHARACTER_ACTIONS::MAX] = {
        "IDLE",
        "MOVE",
        "JUMP"
    };
    ImGui::Text(std::string("CurrentState:" + stateNames[(int)stateMachine.GetCurrentState()]).c_str());

    int index = (int)stateMachine.GetCurrentState();
    ImGui::InputInt("State", &index);
    ImGui::InputFloat("JumpState", &jumpPower);
}
