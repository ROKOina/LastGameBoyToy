#include "CharacterCom.h"
#include "../MovementCom.h"
#include "../CameraCom.h"
#include "../TransformCom.h"

#include "Input\Input.h"
#include "../../GameSource/Math/Mathf.h"

void CharacterCom::Update(float elapsedTime)
{
    ////カメラが向いている方向へ旋回
    //GameObj cameraObj = SceneManager::Instance().GetActiveCamera();
    //std::shared_ptr<CameraCom> cameraCom = cameraObj->GetComponent<CameraCom>();
    //DirectX::XMFLOAT3 cameraForward = cameraCom->GetFront();
    //cameraForward.y = 0;

    //GetGameObject()->transform_->SetRotation(QuaternionStruct::LookRotation(cameraForward).dxFloat4);
    //GetGameObject()->transform_->UpdateTransform();
    //GetGameObject()->transform_->SetUpTransform({ 0,1,0 });


    //ステックのアングル取得
    stickAngle = DirectX::XMConvertToDegrees(atan2(leftStick.y, leftStick.x));

    //ステックの角度制限
    if (stickAngle < 0.0f)
    {
        stickAngle += 360.0f;
    }

    //死亡処理
    if (hitPoint <= 0)
    {
        GetGameObject()->GetComponent<MovementCom>()->AddForce({ 0, 10.0f, 0 });
        return;
    }

    //ステート処理
    attackStateMachine.Update(elapsedTime);
    if (useMoveFlag)moveStateMachine.Update(elapsedTime);

#ifdef _DEBUG

    int inputNum = GetButtonDown();

    //デバッグ中は2つのボタン同時押しで攻撃（画面見づらくなるの防止用
    if (CharacterInput::MainAttackButton & GetButtonDown()
        && GamePad::BTN_LEFT_SHOULDER & GetButton())
    {
        MainAttackDown();
    }
    else if (CharacterInput::MainAttackButton & GetButton()
        && GamePad::BTN_LEFT_SHOULDER & GetButton())
    {
        MainAttackPushing();
    }

    ////デバッグ中は2つのボタン同時押しで攻撃（画面見づらくなるの防止用
    //if (CharacterInput::SubAttackButton & GetButtonDown()
    //    && GamePad::BTN_RIGHT_SHOULDER & GetButton())
    //{
    //    SubAttack();
    //}

    if (CharacterInput::SubAttackButton & GetButtonDown())
    {
        SubAttackDown();
    }
    else if (CharacterInput::SubAttackButton & GetButton())
    {
        SubAttackPushing();
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
    if (CharacterInput::LeftShiftButton & GetButtonDown())
    {
        LeftShiftSkill();
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
    float hp = hitPoint;
    ImGui::DragFloat("HP", &hp);

    int s = (int)(moveStateMachine.GetCurrentState());
    ImGui::InputInt("moveS", &s);
    s = (int)(attackStateMachine.GetCurrentState());
    ImGui::InputInt("attackS", &s);
    moveStateMachine.ImGui();
    attackStateMachine.ImGui();

    ImGui::InputFloat("StickAngle", &stickAngle);

    int i = userInput;
    ImGui::InputInt("input", &i);
    i = userInputDown;
    ImGui::InputInt("userInputDown", &i);
    i = userInputUp;
    ImGui::InputInt("userInputUp", &i);

    ImGui::DragFloat3("fpsCameraDir", &fpsCameraDir.x);
    ImGui::InputInt("netID", &netID);
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

        //マウスカーソルを取得
        POINT cursor;
        ::GetCursorPos(&cursor);
        DirectX::XMFLOAT2 newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));
        ::SetCursorPos(500, 500);

        //動かす速度(感度)
        float moveX = (newCursor.x - 500) * 0.02f;
        float moveY = (newCursor.y - 500) * 0.02f;

        //Y軸回転(ここでオブジェクトの回転)
        DirectX::XMFLOAT3 euler = GetGameObject()->transform_->GetEulerRotation();
        euler.y += moveX * 8.0f;
        GetGameObject()->transform_->SetEulerRotation(euler);

        //X軸回転(カメラのTransformを回転)
        std::shared_ptr<GameObject> cameraplayer = GameObjectManager::Instance().Find("cameraPostPlayer");
        DirectX::XMFLOAT3 cameraeuler = cameraplayer->transform_->GetEulerRotation();
        cameraeuler.x += moveY * 5.0f;
        cameraplayer->transform_->SetEulerRotation(cameraeuler);

        //回転制御
        if (cameraeuler.x > 70)
        {
            cameraeuler.x = 70;
            cameraplayer->transform_->SetEulerRotation(cameraeuler);
        }
        if (cameraeuler.x < -70)
        {
            cameraeuler.x = -70;
            cameraplayer->transform_->SetEulerRotation(cameraeuler);
        }
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