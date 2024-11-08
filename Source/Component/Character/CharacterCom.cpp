#include "CharacterCom.h"
#include "CharaStatusCom.h"
#include "Component/Camera/CameraCom.h"
#include "Component/System/TransformCom.h"
#include "Input\Input.h"
#include "Math/Mathf.h"
#include "Component\PostEffect\PostEffect.h"
#include "Setting/Setting.h"

void CharacterCom::Update(float elapsedTime)
{
    StanUpdate(elapsedTime);

    //�X�e�b�N�̊p�x�擾
    stickAngle = DirectX::XMConvertToDegrees(atan2(leftStick.y, leftStick.x));

    //�X�e�b�N�̊p�x����
    if (stickAngle < 0.0f)
    {
        stickAngle += 360.0f;
    }

    //���݂̃A�j���[�V�����Ɏg�p���Ă���p�x
    nowAngle = InterpolateAngle(nowAngle, stickAngle, elapsedTime, lerpSpeed);

    //�X�e�[�g����
    if (!GetGameObject()->GetComponent<CharaStatusCom>()->IsDeath())
        attackStateMachine.Update(elapsedTime);
    else
        if (moveStateMachine.GetCurrentState() != CHARACTER_MOVE_ACTIONS::DEATH)    //���S����
            moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::DEATH);
    if (useMoveFlag)moveStateMachine.Update(elapsedTime);

#ifdef _DEBUG

    int inputNum = GetButtonDown();

    //�f�o�b�O����2�̃{�^�����������ōU���i��ʌ��Â炭�Ȃ�̖h�~�p
    if (CharacterInput::MainAttackButton & GetButtonDown()
        && GamePad::BTN_LEFT_SHOULDER & GetButton())
    {
        //�E���g���͍U�����ς��
        if (!isUseUlt)
            MainAttackDown();
        else
        {
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

    //�_�b�V���X�L��
    SetLSSkillCoolTime(dashRecast);
    if (CharacterInput::LeftShiftButton & GetButton()
        && LScool.timer >= LScool.time && dashGauge >= 0 && IsPushLeftStick())
    {
        //�Q�[�W���炷
        dashGauge -= dashGaugeMinus * elapsedTime;
        //�_�b�V������
        LeftShiftSkill(elapsedTime);

        //�_�b�V������񂾂�����
        const auto& posteffect = GameObjectManager::Instance().Find("posteffect");
        if (!dashFlag)
        {
            posteffect->GetComponent<PostEffect>()->SetParameter(0.4f, 50.0f, PostEffect::PostEffectParameter::BlurStrength);
            GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->CameraShake(0.005f, 0.5f);
            dashFlag = true;
            dashGauge -= 2; //�ŏ��͈�C�Ɍ��炷
        }
        else
        {
            posteffect->GetComponent<PostEffect>()->SetParameter(0.0f, 1.0f, PostEffect::PostEffectParameter::BlurStrength);
        }

        //�Q�[�W���Ȃ��Ȃ�����^�C�}�[���Z�b�g
        if (dashGauge <= 0)
        {
            LScool.timer = 0;
            posteffect->GetComponent<PostEffect>()->SetParameter(0.0f, 1.0f, PostEffect::PostEffectParameter::BlurStrength);
        }
    }
    else
    {
        //�_�b�V���I�����Ɉ�x����
        if (dashFlag)
        {
            dashFlag = false;
            dashSpeed = dashSpeedFirst; //������ݒ�
            dashFirstTimer = dashFirstTime; //�����^�C�}�[��������
        }

        //�Q�[�W���₷
        dashGauge += dashGaugePlus * elapsedTime;
        if (dashGauge > dashGaugeMax)
        {
            dashGauge = dashGaugeMax;
        }
    }

    //�u���[
    //GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->ParameterMove(elapsedTime, 0.5f, fastDash, PostEffect::PostEffectParameter::BlurStrength);

    if (CharacterInput::JumpButton_SPACE & GetButtonDown())
    {
        Spacecool.timer = 0;
        SpaceSkill();
    }

    //�쑺�ǉ� R�L�[
    if (CharacterInput::UltimetButton_R & GetButtonDown()
        /*&& Rcool.timer >= Rcool.time*/)
    {
        //Rcool.timer = 0;
        //UltSkill();

        //�E���g�����t���OON
        if (isMaxUlt)
        {
            SetRSkillCoolTime(0.5f);
            isUseUlt = true;
            isMaxUlt = false;
            attackUltCounter = 0;
            ultGauge = 0;
        }
    }

    //�E���g�X�V
    ultGauge += elapsedTime;
    if (ultGauge >= ultGaugeMax)
    {
        isMaxUlt = true;
        ultGauge = ultGaugeMax;
    }

    //�N�[���_�E���X�V
    CoolUpdate(elapsedTime);

    //�J��������
    CameraControl();
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
        ImGui::DragFloat("RTime", &Rcool.time);
        ImGui::DragFloat("RTimer", &Rcool.timer);

        ImGui::TreePop();
    }
}

//�_�b�V��
void CharacterCom::LeftShiftSkill(float elapsedTime)
{
    //�ő呬�x�ő�����ς���
    auto& moveCmp = GetGameObject()->GetComponent<MovementCom>();
    float maxSpeed = moveCmp->GetMoveMaxSpeed();
    maxSpeed += dashSpeed;
    moveCmp->SetAddMoveMaxSpeed(maxSpeed);

    //��������
    dashFirstTimer -= elapsedTime;
    if (dashFirstTimer < 0)
    {
        //���x�𕁒ʂ�
        dashSpeed = dashSpeedNormal;
    }
}

void CharacterCom::CameraControl()
{
    if (!cameraObj)return;
    GamePad& gamePad = Input::Instance().GetGamePad();

    //�Q�[���J�����̏ꍇ
    if (cameraObj->GetComponent<CameraCom>()->GetIsActive())
    {
        //�t���[�ɐ؂�ւ�
        if (GamePad::BTN_LCONTROL & gamePad.GetButtonDown())
        {
            std::shared_ptr<GameObject> g = GameObjectManager::Instance().Find("freecamera");
            g->GetComponent<CameraCom>()->ActiveCameraChange();
            return;
        }

        //�ݒ��ʂ��J��(P)
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

        //UI�z�u���̓}�E�X���Œ肵�Ȃ�
        if (!cameraObj->GetComponent<CameraCom>()->GetIsUiCreate())
        {
            //�}�E�X�J�[�\�����擾
            POINT cursor;
            ::GetCursorPos(&cursor);
            DirectX::XMFLOAT2 newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));
            ::SetCursorPos(500, 500);

            //���������x(���x)
            auto& ss = SceneManager::Instance().GetSettingScreen();
            float moveX = (newCursor.x - 500) * 0.0005f * ss->GetSensitivity();
            float moveY = (newCursor.y - 500) * 0.0005f * ss->GetSensitivity();

            //Y����](�����ŃI�u�W�F�N�g�̉�])
            DirectX::XMFLOAT3 euler = GetGameObject()->transform_->GetEulerRotation();
            euler.y += moveX * 8.0f;
            GetGameObject()->transform_->SetEulerRotation(euler);

            //X����](�J������Transform����])
            std::shared_ptr<GameObject> cameraplayer = GameObjectManager::Instance().Find("cameraPostPlayer");
            DirectX::XMFLOAT3 cameraeuler = cameraplayer->transform_->GetEulerRotation();
            cameraeuler.x += moveY * 5.0f;
            cameraplayer->transform_->SetEulerRotation(cameraeuler);

            //��]����
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
        //�L�����J�����ɐ؂�ւ�
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

    //�^�C�}�[����
    if (stanTimer > 0)
    {
        isStan = true;
        stanTimer -= elapsedTime;
    }

    if (!isStan)return;

    //�X�^�����Ȃ�

    //���͎󂯕t���Ȃ�
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

float CharacterCom::Lerp(float start, float end, float t)
{
    return start + t * (end - start);
}

float CharacterCom::InterpolateAngle(float currentAngle, float targetAngle, float deltaTime, float speed)
{
    // �p�x�̍����v�Z
    float diff = targetAngle - currentAngle;

    // ����180�x�𒴂���ꍇ�A�t�����Ōv�Z����
    if (diff > 180.0f) {
        diff -= 360.0f;
    }
    else if (diff < -180.0f) {
        diff += 360.0f;
    }

    // �������߂Â���iLerp ��p����j
    currentAngle = Lerp(currentAngle, currentAngle + diff, deltaTime * speed);

    //�X�e�b�N�̊p�x����
    if (currentAngle < 0.0f)
    {
        currentAngle += 360.0f;
    }

    //�X�e�b�N�̊p�x����
    if (currentAngle > 360.0f)
    {
        currentAngle = 0.0f;
    }

    return currentAngle;
}