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
        //�ݒ��ʂ��J��(P)
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

    //�ݒ��ʒ��͑���o���Ȃ�
    bool isDash = false;
    if (!isViewSetting)
    {
        //�X�e�b�N�̊p�x�擾
        stickAngle = DirectX::XMConvertToDegrees(atan2(leftStick.y, leftStick.x));

        //�X�e�b�N�̊p�x����
        if (stickAngle < 0.0f)
        {
            stickAngle += 360.0f;
        }

        //���݂̃A�j���[�V�����Ɏg�p���Ă���p�x
        nowAngle = InterpolateAngle(nowAngle, stickAngle, elapsedTime, lerpSpeed);

        //���̓X�e�[�g�X�V
        InputStateUpdate(elapsedTime);

        //�_�b�V��
        isDash = DashUpdateReIsDash(elapsedTime);

        //�J��������
        CameraControl();
    }

    //�_�b�V���֌W
    if (dashFlag)
        DashFewSub(elapsedTime);
    if (!isDash)
    {
        dashDraceTimer -= elapsedTime;
        if (dashDraceTimer < 0)
        {
            //�_�b�V���I�����Ɉ�x����
            if (dashFlag)
            {
                dashFlag = false;
                dashSpeed = dashSpeedFirst; //������ݒ�
                dashFirstTimer = dashFirstTime; //�����^�C�}�[��������
            }

            //�Q�[�W���₷
            //dashGauge += dashGaugePlus * elapsedTime;
            //if (dashGauge > dashGaugeMax)
            //{
            //    dashGauge = dashGaugeMax;
            //}
        }
    }
    else
        dashDraceTimer = dashDraceTime; //�_�b�V���P�\����

    //�E���g�X�V
    UltUpdate(elapsedTime);

    //�N�[���_�E���X�V
    CoolUpdate(elapsedTime);

    //�_���[�W�r�l�b�g����
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

//�_�b�V��
void CharacterCom::DashFewSub(float elapsedTime)
{
    if (std::string(GetGameObject()->GetName()) != "player")return;

    //�ő呬�x�ő�����ς���
    auto& moveCmp = GetGameObject()->GetComponent<MovementCom>();
    moveCmp->SetAddMoveMaxSpeed(dashSpeed);

    //��������
    dashFirstTimer -= elapsedTime;
    if (dashFirstTimer < 0)
    {
        //���x�𕁒ʂ�
        dashSpeed = dashSpeedNormal;
    }
}

void CharacterCom::InputStateUpdate(float elapsedTime)
{
    //�X�e�[�g����
    if (!GetGameObject()->GetComponent<CharaStatusCom>()->IsDeath())
        attackStateMachine.Update(elapsedTime);
    else
        if (moveStateMachine.GetCurrentState() != CHARACTER_MOVE_ACTIONS::DEATH)    //���S����
            moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::DEATH);
    if (useMoveFlag)moveStateMachine.Update(elapsedTime);

    int inputNum = GetButtonDown();

#ifdef _DEBUG
    //�f�o�b�O����2�̃{�^�����������ōU���i��ʌ��Â炭�Ȃ�̖h�~�p
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
    //�f�o�b�O����2�̃{�^�����������ōU���i��ʌ��Â炭�Ȃ�̖h�~�p
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

    //�쑺�ǉ� R�L�[
    if (CharacterInput::UltimetButton_R & GetButtonDown()
        /*&& Rcool.timer >= Rcool.time*/)
    {
        //�E���g�����t���OON
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

    //�Q�[���J�����̏ꍇ
    if (cameraObj->GetComponent<CameraCom>()->GetIsActive())
    {
#ifdef _DEBUG
        //�t���[�ɐ؂�ւ�
        if (GamePad::BTN_LCONTROL & gamePad.GetButtonDown())
        {
            std::shared_ptr<GameObject> g = GameObjectManager::Instance().Find("freecamera");
            g->GetComponent<CameraCom>()->ActiveCameraChange();
            return;
        }
#endif

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

bool CharacterCom::DashUpdateReIsDash(float elapsedTime)
{
    bool isNowPush = CharacterInput::LeftShiftButton & GetButtonDown();

    //�r�؂ꂽ�����Ȃ��悤��
    if (!isNowPush && !dashFlag)return false;

    //�_�b�V���X�L��
    SetSkillCoolTime(SkillCoolID::LeftShift, dashRecast);
    if (CharacterInput::LeftShiftButton & GetButton()
        && IsSkillCoolMax(SkillCoolID::LeftShift) && dashGauge >= 0 && IsPushLeftStick())
    {
        //�Q�[�W���炷
        dashGauge -= dashGaugeMinus * elapsedTime;

        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::BlurStrength };

        //�_�b�V������񂾂�����
        const auto& posteffect = GameObjectManager::Instance().Find("posteffect");
        if (isNowPush)
        {
            posteffect->GetComponent<PostEffect>()->SetParameter(0.4f, 50.0f, parameters);
            dashFlag = true;
            dashGauge -= 5; //�ŏ��͈�C�Ɍ��炷

            //��
            GetGameObject()->GetComponent<AudioCom>()->Play("P_DASH", false, 10);
        }
        else
        {
            posteffect->GetComponent<PostEffect>()->SetParameter(0.0f, 1.0f, parameters);
        }

        //�Q�[�W���Ȃ��Ȃ�����^�C�}�[���Z�b�g
        if (dashGauge <= 0)
        {
            skillCools[SkillCoolID::LeftShift].timer = 0;
            posteffect->GetComponent<PostEffect>()->SetParameter(0.0f, 1.0f, parameters);
        }

        return true;
    }

    return false;
}

//�r�l�b�g����
void CharacterCom::Vinetto(float elapsedTime)
{
    float previousHP = GetGameObject()->GetComponent<CharaStatusCom>()->GetMaxHitpoint(); // �ő�HP
    float currentHP = *GetGameObject()->GetComponent<CharaStatusCom>()->GetHitPoint();    // ����HP

    if (const auto posteffect = GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>())
    {
        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::VignetteIntensity };

        // HP�����������ꍇ�̂݃r�l�b�g���ʂ𔭓�
        if (previousHP - currentHP > 0)
        {
            posteffect->SetParameter(0.99f, 130.0f, parameters); // �����r�l�b�g���ʂ�ݒ�

            //�C�[�W���O�v���C
            if (GameObjectManager::Instance().Find("HpGauge"))
                GameObjectManager::Instance().Find("HpGauge")->GetComponent<Sprite>()->EasingPlay();

            //��
            GetGameObject()->GetComponent<AudioCom>()->Play("P_DAMAGE", false, 10);
        }
        else
        {
            posteffect->SetParameter(0.01f, 8.0f, parameters); // ���ɖ߂�
        }
    }

    // ���݂�HP������p�ɕۑ�
    GetGameObject()->GetComponent<CharaStatusCom>()->SetMaxHitPoint(currentHP);
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
    //�Q�[�W�X�V
    ultGauge += elapsedTime;
    if (ultGauge >= ultGaugeMax)
    {
        isMaxUlt = true;
        ultGauge = ultGaugeMax;
    }
    else
    {
        isMaxUlt = false; // max�����B�Ȃ�false�ɖ߂�
    }

    //��O����
    const auto& ultui = GameObjectManager::Instance().Find("UltFrame");
    if (ultui != nullptr)
    {
        if (isMaxUlt && !prevIsMaxUlt)
        {
            GameObjectManager::Instance().Find("UltFrame")->GetComponent<Sprite>()->EasingPlay();
        }

        // isMaxUlt �� false �܂��͕ω����Ȃ��ꍇ�� StopEasing ���Ă�
        if (!isMaxUlt)
        {
            GameObjectManager::Instance().Find("UltFrame")->GetComponent<Sprite>()->StopEasing();
        }
    }

    // ��Ԃ��L�^
    prevIsMaxUlt = isMaxUlt;
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