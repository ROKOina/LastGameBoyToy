#include "CharacterCom.h"
#include "../MovementCom.h"
#include "../CameraCom.h"
#include "../TransformCom.h"

#include "Input\Input.h"
#include "../../GameSource/Math/Mathf.h"

void CharacterCom::Update(float elapsedTime)
{
    //�J�����������Ă�������֐���
    DirectX::XMFLOAT3 cameraForward = SceneManager::Instance().GetActiveCamera()->GetComponent<CameraCom>()->GetFront();
    GetGameObject()->transform_->SetRotation(QuaternionStruct::LookRotation(cameraForward).dxFloat4);
    GetGameObject()->transform_->UpdateTransform();
    GetGameObject()->transform_->SetUpTransform({ 0,1,0 });

    stateMachine.Update(elapsedTime);

#ifdef _DEBUG
    if (CharacterInput::MainAttackButton & GetButtonDown()
    && GamePad::BTN_LEFT_SHOULDER & GetButton())
    {
        MainAttack();
    }

#else
    if (CharacterInput::MainAttackButton & GetButtonDown())
    {
        MainAttack();
    }

#endif // _DEBUG

    if (CharacterInput::MainSkillButton_Q & GetButtonDown())
    {
        MainSkill();
    }
    if (CharacterInput::SubSkillButton_E & GetButtonDown())
    {
        SubSkill();
    }
    if (CharacterInput::JumpButton_SPACE & GetButtonDown())
    {
        SpaceSkill();
    }

    //�J��������
    CameraControl();
}

void CharacterCom::OnGUI()
{
    std::string stateNames[(int)CHARACTER_ACTIONS::MAX] = {
        "IDLE",
        "MOVE",
        "DASH",
        "JUMP",
        "ATTACK"
    };
    ImGui::Text(std::string("CurrentState:" + stateNames[(int)stateMachine.GetCurrentState()]).c_str());

    int index = (int)stateMachine.GetCurrentState();
    ImGui::InputInt("State", &index);
    ImGui::InputFloat("JumpState", &jumpPower);

    //ImGui�\��
    ImGui::Separator();

    static int drawState = 0;
    ImGui::Text(std::string("DebugDrawState:" + stateNames[drawState]).c_str());
    if (ImGui::InputInt("drawState", &drawState))
    {
        if (drawState >= (int)CHARACTER_ACTIONS::MAX)
            drawState = (int)CHARACTER_ACTIONS::MAX - 1;
        if (drawState < 0)drawState = 0;
    }

    if (!stateMachine.CurrentStateImGui((CHARACTER_ACTIONS)drawState))
    {
        ImGui::Text("not found");
    }
}

void CharacterCom::CameraControl()
{
    std::shared_ptr<GameObject> cameraObj = SceneManager::Instance().GetActiveCamera();

    //�Q�[���J�����̏ꍇ
    if (std::strcmp(cameraObj->GetName(), "normalcamera") == 0)
    {
        //�t���[�ɐ؂�ւ�
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

        //��]
        DirectX::XMFLOAT3 euler = cameraPost->transform_->GetEulerRotation();
        euler.y += moveX * 8.0f;
        euler.x += moveY * 5.0f;
        cameraPost->transform_->SetEulerRotation(euler);
        //�c������
        euler = cameraPost->transform_->GetEulerRotation();
        if (euler.x > 70)
        {
            euler.x = 70;
            cameraPost->transform_->SetEulerRotation(euler);
        }
        if (euler.x < -70)
        {
            euler.x = -70;
            cameraPost->transform_->SetEulerRotation(euler);
        }

        //�ʒu
        cameraPost->transform_->SetWorldPosition(GetGameObject()->transform_->GetWorldPosition());

        cameraPost->transform_->UpdateTransform();


    }
}
