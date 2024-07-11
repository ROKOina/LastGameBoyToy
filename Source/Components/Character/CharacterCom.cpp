#include "CharacterCom.h"
#include "../MovementCom.h"
#include "../CameraCom.h"
#include "../TransformCom.h"

#include "Input\Input.h"
#include "../../GameSource/Math/Mathf.h"

void CharacterCom::Update(float elapsedTime)
{
    //カメラが向いている方向へ旋回
    DirectX::XMFLOAT3 cameraForward = SceneManager::Instance().GetActiveCamera()->GetComponent<CameraCom>()->GetFront();
    cameraForward.y = 0;
    
    GetGameObject()->transform_->SetRotation(QuaternionStruct::LookRotation(cameraForward).dxFloat4);
    GetGameObject()->transform_->UpdateTransform();
    GetGameObject()->transform_->SetUpTransform({ 0,1,0 });

    attackStateMachine.Update(elapsedTime);
    if(useMoveFlag)moveStateMachine.Update(elapsedTime);

#ifdef _DEBUG

    int inputNum = GetButtonDown();

    //デバッグ中は2つのボタン同時押しで攻撃（画面見づらくなるの防止用
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

    //野村追加 Rキー
    if (CharacterInput::UltimetButton_R & GetButtonDown())
    {
        UltSkill();
    }


    //カメラ制御
    CameraControl();
}

void CharacterCom::OnGUI()
{
    moveStateMachine.ImGui();
    attackStateMachine.ImGui();
}

void CharacterCom::CameraControl()
{
    if (!cameraObj)return;
    GamePad& gamePad = Input::Instance().GetGamePad();

    //ゲームカメラの場合
    if (cameraObj->GetComponent<CameraCom>()->GetIsActive())
    {
        //フリーに切り替え
        if (GamePad::BTN_LCONTROL & gamePad.GetButtonDown())
        {
            std::shared_ptr<GameObject> g = GameObjectManager::Instance().Find("freecamera");
            g->GetComponent<CameraCom>()->ActiveCameraChange();
            return;
        }

        std::shared_ptr<CameraCom> camera = cameraObj->GetComponent<CameraCom>();

        POINT cursor;
        ::GetCursorPos(&cursor);

        DirectX::XMFLOAT2 newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

        ::SetCursorPos(500, 500);

        float moveX = (newCursor.x - 500) * 0.02f;
        float moveY = (newCursor.y - 500) * 0.02f;

        //std::shared_ptr<GameObject> cameraPost = GameObjectManager::Instance().Find("cameraPostPlayer");

        //回転
        DirectX::XMFLOAT3 euler = GetGameObject()->transform_->GetEulerRotation();
        euler.y += moveX * 8.0f;
        //euler.x += moveY * 5.0f;

        //回転制御
        if (euler.x > 70)
        {
            euler.x = 70;
            GetGameObject()->transform_->SetEulerRotation(euler);
        }
        if (euler.x < -70)
        {
            euler.x = -70;
            GetGameObject()->transform_->SetEulerRotation(euler);
        }

        GetGameObject()->transform_->SetEulerRotation(euler);
    }
    else
    {
        //キャラカメラに切り替え
        if (GamePad::BTN_LCONTROL & gamePad.GetButtonDown())
        {
            cameraObj->GetComponent<CameraCom>()->ActiveCameraChange();
            return;
        }
    }
}
