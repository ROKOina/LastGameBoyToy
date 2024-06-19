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

    if (CharacterInput::MainSkillButton_Q & gamePad.GetButtonDown())
    {
        MainSkill();
    }
    if (CharacterInput::SubSkillButton_E & gamePad.GetButtonDown())
    {
        SubSkill();
    }

    //カメラ制御
    CameraControl();
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

void CharacterCom::CameraControl()
{
    std::shared_ptr<GameObject> cameraObj = SceneManager::Instance().GetActiveCamera();

    //ゲームカメラの場合
    if (std::strcmp(cameraObj->GetName(), "normalcamera") == 0)
    {
        //フリーに切り替え
        if (::GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
            std::shared_ptr<GameObject> g = GameObjectManager::Instance().Find("freecamera");
            g->GetComponent<CameraCom>()->SetActiveInitialize();
            return;
        }

        std::shared_ptr<CameraCom> camera = cameraObj->GetComponent<CameraCom>();

        POINT cursor;
        ::GetCursorPos(&cursor);

        DirectX::XMFLOAT2 newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

        ::SetCursorPos(500, 500);

        float moveX = (newCursor.x - 500) * 0.02f;
        float moveY = (newCursor.y - 500) * 0.02f;

        std::shared_ptr<GameObject> cameraPost = GameObjectManager::Instance().Find("cameraPostPlayer");

        //回転
        DirectX::XMFLOAT3 euler = cameraPost->transform_->GetEulerRotation();
        euler.y += moveX * 8.0f;
        euler.x += moveY * 5.0f;
        cameraPost->transform_->SetEulerRotation(euler);

        //位置
        cameraPost->transform_->SetWorldPosition(GetGameObject()->transform_->GetWorldPosition());
        
        cameraPost->transform_->UpdateTransform();


    }
}
