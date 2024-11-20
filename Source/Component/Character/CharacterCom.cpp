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

void CharacterCom::Update(float elapsedTime)
{
    StanUpdate(elapsedTime);

    //ステックの角度取得
    stickAngle = DirectX::XMConvertToDegrees(atan2(leftStick.y, leftStick.x));

    //ステックの角度制限
    if (stickAngle < 0.0f)
    {
        stickAngle += 360.0f;
    }

    //現在のアニメーションに使用している角度
    nowAngle = InterpolateAngle(nowAngle, stickAngle, elapsedTime, lerpSpeed);

    //ステート処理
    if (!GetGameObject()->GetComponent<CharaStatusCom>()->IsDeath())
        attackStateMachine.Update(elapsedTime);
    else
        if (moveStateMachine.GetCurrentState() != CHARACTER_MOVE_ACTIONS::DEATH)    //死亡処理
            moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::DEATH);
    if (useMoveFlag)moveStateMachine.Update(elapsedTime);

#ifdef _DEBUG

    int inputNum = GetButtonDown();

    //デバッグ中は2つのボタン同時押しで攻撃（画面見づらくなるの防止用
    if (CharacterInput::MainAttackButton & GetButtonDown()
        && GamePad::BTN_LEFT_SHOULDER & GetButton())
    {
        //ウルト中は攻撃が変わる
        if (!isUseUlt)
            MainAttackDown();
        else
        {
            //ウルト中
            if (Rcool.timer >= Rcool.time)
            {
                Rcool.timer = 0;
                UltSkill();
                attackUltCounter++;
                if (attackUltCounter >= attackUltCountMax)
                    isUseUlt = false;
            }
        }
    }
    else if (CharacterInput::MainAttackButton & GetButton()
        && GamePad::BTN_LEFT_SHOULDER & GetButton())
    {
        if (!isUseUlt)
            MainAttackPushing();
    }

    if (CharacterInput::SubAttackButton & GetButtonDown()
        && LeftClickcool.timer >= LeftClickcool.time)
    {
        LeftClickcool.timer = 0;
        SubAttackDown();
    }
    else if (CharacterInput::SubAttackButton & GetButton())
    {
        SubAttackPushing();
    }

#else
    if (CharacterInput::MainAttackButton & GetButtonDown())
    {
        //MainAttack();
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
        && LScool.timer >= LScool.time && dashGauge >= 0 && IsPushLeftStick())
    {
        //ゲージ減らす
        dashGauge -= dashGaugeMinus * elapsedTime;
        //ダッシュ処理
        LeftShiftSkill(elapsedTime);

        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::BlurStrength };

        //ダッシュ時一回だけ入る
        const auto& posteffect = GameObjectManager::Instance().Find("posteffect");
        if (!dashFlag)
        {
            posteffect->GetComponent<PostEffect>()->SetParameter(0.4f, 50.0f, parameters);
            dashFlag = true;
            dashGauge -= 5; //最初は一気に減らす
        }
        else
        {
            posteffect->GetComponent<PostEffect>()->SetParameter(0.0f, 1.0f, parameters);
        }

        //ゲージがなくなったらタイマーをセット
        if (dashGauge <= 0)
        {
            LScool.timer = 0;
            posteffect->GetComponent<PostEffect>()->SetParameter(0.0f, 1.0f, parameters);
        }
    }
    else
    {
        //ダッシュ終了時に一度入る
        if (dashFlag)
        {
            dashFlag = false;
            dashSpeed = dashSpeedFirst; //初速を設定
            dashFirstTimer = dashFirstTime; //初速タイマーを初期化
        }

        //ゲージ増やす
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
        /*&& Rcool.timer >= Rcool.time*/)
    {
        //Rcool.timer = 0;
        //UltSkill();

        //ウルト発動フラグON
        if (isMaxUlt)
        {
            SetRSkillCoolTime(0.5f);
            isUseUlt = true;
            isMaxUlt = false;
            attackUltCounter = 0;
            ultGauge = 0;
        }
    }

    //ウルト更新
    UltUpdate(elapsedTime);

    //クールダウン更新
    CoolUpdate(elapsedTime);

    //カメラ制御
    CameraControl();

    //ビネット発動
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

        ImGui::DragInt("attackUltCountMax", &attackUltCountMax);
        ImGui::DragInt("attackUltCounter", &attackUltCounter);

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
        ImGui::InputInt("netID", &netID);

        ImGui::TreePop();
    }

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
        ImGui::DragFloat("LeftClickTime", &LeftClickcool.time);
        ImGui::DragFloat("LeftClickTimer", &LeftClickcool.timer);
        ImGui::Separator();
        ImGui::DragFloat("RTime", &Rcool.time);
        ImGui::DragFloat("RTimer", &Rcool.timer);

        ImGui::TreePop();
    }
}

//ダッシュ
void CharacterCom::LeftShiftSkill(float elapsedTime)
{
    if (std::string(GetGameObject()->GetName()) != "player")return;

    //最大速度で速さを変える
    auto& moveCmp = GetGameObject()->GetComponent<MovementCom>();
    float maxSpeed = moveCmp->GetMoveMaxSpeed();
    maxSpeed += dashSpeed;
    moveCmp->SetAddMoveMaxSpeed(maxSpeed);

    //初速減衰
    dashFirstTimer -= elapsedTime;
    if (dashFirstTimer < 0)
    {
        //速度を普通に
        dashSpeed = dashSpeedNormal;
    }
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

        //設定画面を開く(P)
        auto& ss = SceneManager::Instance().GetSettingScreen();
        bool isViewSetting = ss->IsViewSetting();
        if (GamePad::BTN_P & gamePad.GetButtonDown())
        {
            if (isViewSetting)
            {
                ss->SetViewSetting(false);
            }
            else
            {
                ss->SetViewSetting(true);
            }
        }
        if (isViewSetting)return;

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

//ビネット効果
void CharacterCom::Vinetto(float elapsedTime)
{
    float previousHP = GetGameObject()->GetComponent<CharaStatusCom>()->GetMaxHitpoint(); // 最大HP
    float currentHP = *GetGameObject()->GetComponent<CharaStatusCom>()->GetHitPoint();    // 現在HP
    const auto posteffect = GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>();

    if (posteffect)
    {
        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::VignetteIntensity };

        // HPが減少した場合のみビネット効果を発動
        if (previousHP - currentHP > 0)
        {
            posteffect->SetParameter(0.99f, 130.0f, parameters); // 強いビネット効果を設定
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
    Qcool.timer += elapsedTime;
    Ecool.timer += elapsedTime;
    Rcool.timer += elapsedTime;
    LScool.timer += elapsedTime;
    Spacecool.timer += elapsedTime;
    LeftClickcool.timer += elapsedTime;
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

    //ウルトエフェクト
    if (attackUltRayObj.lock())
    {
        auto& rayCol = attackUltRayObj.lock()->GetComponent<RayColliderCom>();
        if (rayCol)
        {
            for (auto& obj : rayCol->OnHitGameObject())
            {
                {
                    std::shared_ptr<GameObject> attackUltEffBomb = GameObjectManager::Instance().Create();
                    attackUltEffBomb->SetName("attackUltEffBomb");
                    attackUltEffBomb->transform_->SetWorldPosition(obj.hitPos);
                    std::shared_ptr<GPUParticle> eff = attackUltEffBomb->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltBombCircle.gpuparticle", 300);
                    eff->Play();
                    attackUltEffBomb->AddComponent<RemoveTimerCom>(3);
                    {
                        std::shared_ptr<GameObject> attackUltEffBomb02 = attackUltEffBomb->AddChildObject();
                        attackUltEffBomb02->SetName("attackUltEffBomb02");
                        std::shared_ptr<GPUParticle> eff = attackUltEffBomb02->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltBombCircleLight.gpuparticle", 100);
                        eff->Play();
                    }
                    {
                        std::shared_ptr<GameObject> attackUltEffBomb03 = attackUltEffBomb->AddChildObject();
                        attackUltEffBomb03->SetName("attackUltEffBomb03");
                        std::shared_ptr<GPUParticle> eff = attackUltEffBomb03->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltBombFire.gpuparticle", 50);
                        eff->Play();
                    }
                }
            }
        }
    }
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