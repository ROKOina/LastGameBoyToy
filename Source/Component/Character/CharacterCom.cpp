#include "CharacterCom.h"
#include "CharaStatusCom.h"
#include "Component/Camera/CameraCom.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Particle/GPUParticle.h"
#include "Input\Input.h"
#include "Math/Mathf.h"
#include "Component\PostEffect\PostEffect.h"
#include "RemoveTimerCom.h"
#include "Setting/Setting.h"
#include "Component\Audio\AudioCom.h"
#include "Component\Sprite\Sprite.h"
#include "Component\Renderer\RendererCom.h"

void CharacterCom::Update(float elapsedTime)
{
    auto& ss = SceneManager::Instance().GetSettingScreen();
    bool isViewSetting = ss->IsViewSetting();
    {
        //設定画面を開く(P)
        GamePad& gamePad = Input::Instance().GetGamePad();
        if (GamePad::BTN_P & gamePad.GetButtonDown())
        {
            if (isViewSetting)
            {
                ss->SetViewSetting(false);
                ::SetCursorPos(500, 500);
            }
            else
            {
                ss->SetViewSetting(true);
            }
        }
    }

    StanUpdate(elapsedTime);

    //設定画面中は操作出来ない
    bool isDash = false;
    if (!isViewSetting)
    {
        //ステックの角度取得
        stickAngle = DirectX::XMConvertToDegrees(atan2(leftStick.y, leftStick.x));

        //ステックの角度制限
        if (stickAngle < 0.0f)
        {
            stickAngle += 360.0f;
        }

        //現在のアニメーションに使用している角度
        nowAngle = InterpolateAngle(nowAngle, stickAngle, elapsedTime, lerpSpeed);

        //入力ステート更新
        InputStateUpdate(elapsedTime);

        //ダッシュ
        isDash = DashUpdateReIsDash(elapsedTime);

        //カメラ制御
        CameraControl();
    }

    //ダッシュ関係
    if (dashFlag)
        DashFewSub(elapsedTime);
    if (!isDash)
    {
        dashDraceTimer -= elapsedTime;
        if (dashDraceTimer < 0)
        {
            //ダッシュ終了時に一度入る
            if (dashFlag)
            {
                dashFlag = false;
                dashSpeed = dashSpeedFirst; //初速を設定
                dashFirstTimer = dashFirstTime; //初速タイマーを初期化
            }

            //ゲージ増やす
            //dashGauge += dashGaugePlus * elapsedTime;
            //if (dashGauge > dashGaugeMax)
            //{
            //    dashGauge = dashGaugeMax;
            //}
        }
    }
    else
        dashDraceTimer = dashDraceTime; //ダッシュ猶予時間

    //ウルト更新
    UltUpdate(elapsedTime);

    //クールダウン更新
    CoolUpdate(elapsedTime);

    //ダメージビネット発動
    Vinetto(elapsedTime);
}

void CharacterCom::OnGUI()
{
    ImGui::Checkbox("isHitAttack", &isHitAttack);
    ImGui::DragFloat("jump", &jumpPower, 0.1f);

    if (ImGui::TreeNode("ult"))
    {
        ImGui::Checkbox("isMaxUlt", &isMaxUlt);
        ImGui::Checkbox("isUseUlt", &isUseUlt);
        ImGui::DragFloat("ultGaugeMax", &ultGaugeMax, 0.1f);
        ImGui::DragFloat("ultGauge", &ultGauge, 0.1f);

        ImGui::Separator();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("dash"))
    {
        ImGui::DragFloat("dashRecast", &dashRecast, 0.1f);
        ImGui::DragFloat("dashGauge", &dashGauge, 0.1f);
        ImGui::DragFloat("dashGaugeMax", &dashGaugeMax, 0.1f);
        ImGui::DragFloat("dashGaugeMinus", &dashGaugeMinus, 0.1f);
        ImGui::DragFloat("dashGaugePlus", &dashGaugePlus, 0.1f);

        ImGui::DragFloat("dashSpeedFirst", &dashSpeedFirst, 0.1f);
        ImGui::DragFloat("dashSpeedNormal", &dashSpeedNormal, 0.1f);
        ImGui::DragFloat("dashFirstTime", &dashFirstTime, 0.1f);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("state"))
    {
        int s = (int)(moveStateMachine.GetCurrentState());
        ImGui::InputInt("moveS", &s);
        s = (int)(attackStateMachine.GetCurrentState());
        ImGui::InputInt("attackS", &s);
        moveStateMachine.ImGui();
        attackStateMachine.ImGui();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("NetInput"))
    {
        bool stan = isStan;
        ImGui::Checkbox("isStan", &stan);
        ImGui::DragFloat("stanTimer", &stanTimer);

        ImGui::InputFloat("StickAngle", &stickAngle);
        ImGui::InputFloat("nowAngle", &nowAngle);

        int i = userInput;
        ImGui::InputInt("input", &i);
        i = userInputDown;
        ImGui::InputInt("userInputDown", &i);
        i = userInputUp;
        ImGui::InputInt("userInputUp", &i);

        ImGui::DragFloat3("fpsCameraDir", &fpsCameraDir.x);
        ImGui::InputInt("netID", &netCharaData.netPlayerID);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("SkillCool"))
    {

        ImGui::DragFloat("QTime", &skillCools[SkillCoolID::Q].time);
        ImGui::DragFloat("QTimer", &skillCools[SkillCoolID::Q].timer);
        ImGui::Separator();
        ImGui::DragFloat("ETime", &skillCools[SkillCoolID::E].time);
        ImGui::DragFloat("ETimer", &skillCools[SkillCoolID::E].timer);
        ImGui::Separator();
        ImGui::DragFloat("LSTime", &skillCools[SkillCoolID::LeftShift].time);
        ImGui::DragFloat("LSTimer", &skillCools[SkillCoolID::LeftShift].timer);
        ImGui::Separator();
        ImGui::DragFloat("SpaceTime", &skillCools[SkillCoolID::Space].time);
        ImGui::DragFloat("SpaceTimer", &skillCools[SkillCoolID::Space].timer);
        ImGui::Separator();
        ImGui::DragFloat("LeftClickTime", &skillCools[SkillCoolID::LeftClick].time);
        ImGui::DragFloat("LeftClickTimer", &skillCools[SkillCoolID::LeftClick].timer);
        ImGui::Separator();
        ImGui::DragFloat("RTime", &skillCools[SkillCoolID::R].time);
        ImGui::DragFloat("RTimer", &skillCools[SkillCoolID::R].timer);

        ImGui::TreePop();
    }
}

//ダッシュ
void CharacterCom::DashFewSub(float elapsedTime)
{
    if (std::string(GetGameObject()->GetName()) != "player")return;

    //最大速度で速さを変える
    auto& moveCmp = GetGameObject()->GetComponent<MovementCom>();
    moveCmp->SetAddMoveMaxSpeed(dashSpeed);

    //初速減衰
    dashFirstTimer -= elapsedTime;
    if (dashFirstTimer < 0)
    {
        //速度を普通に
        dashSpeed = dashSpeedNormal;
    }
}

void CharacterCom::InputStateUpdate(float elapsedTime)
{
    //ステート処理
    if (!GetGameObject()->GetComponent<CharaStatusCom>()->IsDeath())
        attackStateMachine.Update(elapsedTime);
    else
        if (moveStateMachine.GetCurrentState() != CHARACTER_MOVE_ACTIONS::DEATH)    //死亡処理
            moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::DEATH);
    if (useMoveFlag)moveStateMachine.Update(elapsedTime);

    int inputNum = GetButtonDown();

#ifdef _DEBUG
    //デバッグ中は2つのボタン同時押しで攻撃（画面見づらくなるの防止用
    if (CharacterInput::MainAttackButton & GetButtonDown()
        && GamePad::BTN_A & GetButton())
    {
        MainAttackDown();
    }
    else if (CharacterInput::MainAttackButton & GetButton()
        && GamePad::BTN_A & GetButton())
    {
        if (!isUseUlt)
            MainAttackPushing();
    }
#else
    //デバッグ中は2つのボタン同時押しで攻撃（画面見づらくなるの防止用
    if (CharacterInput::MainAttackButton & GetButtonDown())
    {
        MainAttackDown();
    }
    else if (CharacterInput::MainAttackButton & GetButton())
    {
        if (!isUseUlt)
            MainAttackPushing();
    }
#endif // DEBUG_

    if (CharacterInput::SubAttackButton & GetButtonDown()
        && IsSkillCoolMax(SkillCoolID::LeftClick))
    {
        skillCools[SkillCoolID::LeftClick].timer = 0;
        SubAttackDown();
    }
    else if (CharacterInput::SubAttackButton & GetButton())
    {
        SubAttackPushing();
    }

    if (CharacterInput::MainAttackButton & GetButtonDown())
    {
        //MainAttack();
    }

    if (CharacterInput::MainSkillButton_Q & GetButtonDown()
        && IsSkillCoolMax(SkillCoolID::Q))
    {
        skillCools[SkillCoolID::Q].timer = 0;
        MainSkill();
    }
    if (CharacterInput::SubSkillButton_E & GetButtonDown()
        && IsSkillCoolMax(SkillCoolID::E))
    {
        skillCools[SkillCoolID::E].timer = 0;
        SubSkill();
    }

    if (CharacterInput::JumpButton_SPACE & GetButtonDown())
    {
        skillCools[SkillCoolID::Space].timer = 0;
        SpaceSkill();
    }

    if (CharacterInput::JumpButton_SPACE & GetButton())
    {
        SpaceSkillPushing(elapsedTime);
    }

    //野村追加 Rキー
    if (CharacterInput::UltimetButton_R & GetButtonDown()
        /*&& Rcool.timer >= Rcool.time*/)
    {
        //ウルト発動フラグON
        if (isMaxUlt)
        {
            UltSkill();
            isUseUlt = true;
            isMaxUlt = false;
            ultGauge = 0;

        }
    }
}

void CharacterCom::CameraControl()
{
    if (!cameraObj)return;
    GamePad& gamePad = Input::Instance().GetGamePad();

    //ゲームカメラの場合
    if (cameraObj->GetComponent<CameraCom>()->GetIsActive())
    {
#ifdef _DEBUG
        //フリーに切り替え
        if (GamePad::BTN_LCONTROL & gamePad.GetButtonDown())
        {
            std::shared_ptr<GameObject> g = GameObjectManager::Instance().Find("freecamera");
            g->GetComponent<CameraCom>()->ActiveCameraChange();
            return;
        }
#endif

        //UI配置中はマウスを固定しない
        if (!cameraObj->GetComponent<CameraCom>()->GetIsUiCreate())
        {
            //マウスカーソルを取得
            POINT cursor;
            ::GetCursorPos(&cursor);
            DirectX::XMFLOAT2 newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));
            ::SetCursorPos(500, 500);

            //動かす速度(感度)
            auto& ss = SceneManager::Instance().GetSettingScreen();
            float moveX = (newCursor.x - 500) * 0.0005f * ss->GetSensitivity();
            float moveY = (newCursor.y - 500) * 0.0005f * ss->GetSensitivity();

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

bool CharacterCom::DashUpdateReIsDash(float elapsedTime)
{
    bool isNowPush = CharacterInput::LeftShiftButton & GetButtonDown();

    //途切れたら入らないように
    if (!isNowPush && !dashFlag)return false;

    //ダッシュスキル
    SetSkillCoolTime(SkillCoolID::LeftShift, dashRecast);
    if (CharacterInput::LeftShiftButton & GetButton()
        && IsSkillCoolMax(SkillCoolID::LeftShift) && dashGauge >= 0 && IsPushLeftStick())
    {
        //ゲージ減らす
        dashGauge -= dashGaugeMinus * elapsedTime;

        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::BlurStrength };

        //ダッシュ時一回だけ入る
        const auto& posteffect = GameObjectManager::Instance().Find("posteffect");
        if (isNowPush)
        {
            posteffect->GetComponent<PostEffect>()->SetParameter(0.4f, 50.0f, parameters);
            dashFlag = true;
            dashGauge -= 5; //最初は一気に減らす

            //音
            GetGameObject()->GetComponent<AudioCom>()->Play("P_DASH", false, 10);
        }
        else
        {
            posteffect->GetComponent<PostEffect>()->SetParameter(0.0f, 1.0f, parameters);
        }

        //ゲージがなくなったらタイマーをセット
        if (dashGauge <= 0)
        {
            skillCools[SkillCoolID::LeftShift].timer = 0;
            posteffect->GetComponent<PostEffect>()->SetParameter(0.0f, 1.0f, parameters);
        }

        return true;
    }

    return false;
}

//ビネット効果
void CharacterCom::Vinetto(float elapsedTime)
{
    float previousHP = GetGameObject()->GetComponent<CharaStatusCom>()->GetMaxHitpoint(); // 最大HP
    float currentHP = *GetGameObject()->GetComponent<CharaStatusCom>()->GetHitPoint();    // 現在HP

    if (const auto posteffect = GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>())
    {
        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::VignetteIntensity };

        // HPが減少した場合のみビネット効果を発動
        if (previousHP - currentHP > 0)
        {
            posteffect->SetParameter(0.99f, 130.0f, parameters); // 強いビネット効果を設定

            //イージングプレイ
            if (GameObjectManager::Instance().Find("HpGauge"))
                GameObjectManager::Instance().Find("HpGauge")->GetComponent<Sprite>()->EasingPlay();

            //音
            GetGameObject()->GetComponent<AudioCom>()->Play("P_DAMAGE", false, 10);
        }
        else
        {
            posteffect->SetParameter(0.01f, 8.0f, parameters); // 元に戻す
        }
    }

    // 現在のHPを次回用に保存
    GetGameObject()->GetComponent<CharaStatusCom>()->SetMaxHitPoint(currentHP);
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
    for (int i = 0; i < SkillCoolID::MAX; ++i)
    {
        skillCools[i].timer += elapsedTime;
    }
}

float CharacterCom::Lerp(float start, float end, float t)
{
    return start + t * (end - start);
}

void CharacterCom::UltUpdate(float elapsedTime)
{
    //ゲージ更新
    ultGauge += elapsedTime;
    if (ultGauge >= ultGaugeMax)
    {
        isMaxUlt = true;
        ultGauge = ultGaugeMax;
    }
    else
    {
        isMaxUlt = false; // max未到達ならfalseに戻す
    }

    //例外処理
    const auto& ultui = GameObjectManager::Instance().Find("UltFrame");
    if (ultui != nullptr)
    {
        if (isMaxUlt && !prevIsMaxUlt)
        {
            GameObjectManager::Instance().Find("UltFrame")->GetComponent<Sprite>()->EasingPlay();
        }

        // isMaxUlt が false または変化がない場合は StopEasing を呼ぶ
        if (!isMaxUlt)
        {
            GameObjectManager::Instance().Find("UltFrame")->GetComponent<Sprite>()->StopEasing();
        }
    }

    // 状態を記録
    prevIsMaxUlt = isMaxUlt;
}

float CharacterCom::InterpolateAngle(float currentAngle, float targetAngle, float deltaTime, float speed)
{
    // 角度の差を計算
    float diff = targetAngle - currentAngle;

    // 差が180度を超える場合、逆方向で計算する
    if (diff > 180.0f) {
        diff -= 360.0f;
    }
    else if (diff < -180.0f) {
        diff += 360.0f;
    }

    // 少しずつ近づける（Lerp を用いる）
    currentAngle = Lerp(currentAngle, currentAngle + diff, deltaTime * speed);

    //ステックの角度制限
    if (currentAngle < 0.0f)
    {
        currentAngle += 360.0f;
    }

    //ステックの角度制限
    if (currentAngle > 360.0f)
    {
        currentAngle = 0.0f;
    }

    return currentAngle;
}