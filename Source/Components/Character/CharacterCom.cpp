#include "CharacterCom.h"
#include "../MovementCom.h"
#include "../CameraCom.h"
#include "../TransformCom.h"

#include "Input\Input.h"
#include "../../GameSource/Math/Mathf.h"

void CharacterCom::Update(float elapsedTime)
{
    StanUpdate(elapsedTime);

    //ステックのアングル取得
    stickAngle = DirectX::XMConvertToDegrees(atan2(leftStick.y, leftStick.x));

    //ステックの角度制限
    if (stickAngle < 0.0f)
    {
        stickAngle += 360.0f;
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

    if (CharacterInput::MainSkillButton_Q & GetButtonDown() 
        && Qcool.timer >= Qcool.time)
    {
        Qcool.timer = 0;
        MainSkill();
    }
    if (CharacterInput::SubSkillButton_E & GetButtonDown()
        && Ecool.timer >= Ecool.time)
    {
        Ecool.timer = 0;
        SubSkill();
    }
    //ダッシュスキル
    SetLSSkillCoolTime(dashRecast);
    if (CharacterInput::LeftShiftButton & GetButton()
        && LScool.timer >= LScool.time && dashGauge > 0)
    {
        dashGauge -= dashGaugeMinus * elapsedTime;
        LeftShiftSkill();
        dashFlag = true;
    }
    else
    {
        if (dashFlag)
        {
            LScool.timer = 0;
            dashFlag = false;
        }

        dashGauge += dashGaugePlus * elapsedTime;
        if (dashGauge > dashGaugeMax)
        {
            dashGauge = dashGaugeMax;
        }
    }

    if (CharacterInput::JumpButton_SPACE & GetButtonDown())
    {
        Spacecool.timer = 0;
        SpaceSkill();
    }

    //野村追加 Rキー
    if (CharacterInput::UltimetButton_R & GetButtonDown()
        && Rcool.timer >= Rcool.time)
    {
        Rcool.timer = 0;
        UltSkill();
    }

    //クールダウン更新
    CoolUpdate(elapsedTime);

    //カメラ制御
    CameraControl();
}

void CharacterCom::OnGUI()
{
    ImGui::DragFloat("jump", &jumpPower, 0.1f);

    ImGui::DragFloat("dashRecast", &dashRecast, 0.1f);
    ImGui::DragFloat("dashGauge", &dashGauge, 0.1f);
    ImGui::DragFloat("dashGaugeMax", &dashGaugeMax, 0.1f);
    ImGui::DragFloat("dashGaugeMinus", &dashGaugeMinus, 0.1f);
    ImGui::DragFloat("dashGaugePlus", &dashGaugePlus, 0.1f);

    bool stan = isStan;
    ImGui::Checkbox("isStan", &stan);
    ImGui::DragFloat("stanTimer", &stanTimer);

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

    if (ImGui::TreeNode("SkillCool"))
    {
        ImGui::DragFloat("QTime", &Qcool.time);
        ImGui::DragFloat("QTimer", &Qcool.timer);
        ImGui::Separator();
        ImGui::DragFloat("ETime", &Ecool.time);
        ImGui::DragFloat("ETimer", &Ecool.timer);
        ImGui::Separator();
        ImGui::DragFloat("LSTime", &LScool.time);
        ImGui::DragFloat("LSTimer", &LScool.timer);
        ImGui::Separator();
        ImGui::DragFloat("SpaceTime", &Spacecool.time);
        ImGui::DragFloat("SpaceTimer", &Spacecool.timer);
        ImGui::Separator();
        ImGui::DragFloat("RTime", &Rcool.time);
        ImGui::DragFloat("RTimer", &Rcool.timer);

        ImGui::TreePop();
    }
}

//ダッシュ
void CharacterCom::LeftShiftSkill()
{
    auto& moveCmp = GetGameObject()->GetComponent<MovementCom>();
    float maxSpeed = moveCmp->GetMoveMaxSpeed();
    maxSpeed += dashSpeed;
    moveCmp->SetAddMoveMaxSpeed(maxSpeed);
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

void CharacterCom::StanUpdate(float elapsedTime)
{
    isStan = false;

    //タイマー処理
    if (stanTimer > 0)
    {
        isStan = true;
        stanTimer -= elapsedTime;
    }

    if (!isStan)return;

    //スタン中なら

    //入力受け付けない
    userInput = 0;
    userInputDown = 0;
    userInputUp = 0;
    leftStick = { 0,0 };
    rightStick = { 0,0 };
}

void CharacterCom::CoolUpdate(float elapsedTime)
{
    Qcool.timer += elapsedTime;
    Ecool.timer += elapsedTime;
    Rcool.timer += elapsedTime;
    LScool.timer += elapsedTime;
    Spacecool.timer += elapsedTime;
}