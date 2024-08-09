#include "CharacterCom.h"
#include "../MovementCom.h"
#include "../CameraCom.h"
#include "../TransformCom.h"

#include "Input\Input.h"
#include "../../GameSource/Math/Mathf.h"

void CharacterCom::Update(float elapsedTime)
{
    ////�J�����������Ă�������֐���
    //GameObj cameraObj = SceneManager::Instance().GetActiveCamera();
    //std::shared_ptr<CameraCom> cameraCom = cameraObj->GetComponent<CameraCom>();
    //DirectX::XMFLOAT3 cameraForward = cameraCom->GetFront();
    //cameraForward.y = 0;

    //GetGameObject()->transform_->SetRotation(QuaternionStruct::LookRotation(cameraForward).dxFloat4);
    //GetGameObject()->transform_->UpdateTransform();
    //GetGameObject()->transform_->SetUpTransform({ 0,1,0 });


    //�X�e�b�N�̃A���O���擾
    stickAngle = DirectX::XMConvertToDegrees(atan2(leftStick.y, leftStick.x));

    //�X�e�b�N�̊p�x����
    if (stickAngle < 0.0f)
    {
        stickAngle += 360.0f;
    }

    //���S����
    if (hitPoint <= 0)
    {
        GetGameObject()->GetComponent<MovementCom>()->AddForce({ 0, 10.0f, 0 });
        return;
    }

    //�X�e�[�g����
    attackStateMachine.Update(elapsedTime);
    if (useMoveFlag)moveStateMachine.Update(elapsedTime);

#ifdef _DEBUG

    int inputNum = GetButtonDown();

    //�f�o�b�O����2�̃{�^�����������ōU���i��ʌ��Â炭�Ȃ�̖h�~�p
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

    ////�f�o�b�O����2�̃{�^�����������ōU���i��ʌ��Â炭�Ȃ�̖h�~�p
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

    //�쑺�ǉ� R�L�[
    if (CharacterInput::UltimetButton_R & GetButtonDown())
    {
        UltSkill();
    }

    //�J��������
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

        //�}�E�X�J�[�\�����擾
        POINT cursor;
        ::GetCursorPos(&cursor);
        DirectX::XMFLOAT2 newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));
        ::SetCursorPos(500, 500);

        //���������x(���x)
        float moveX = (newCursor.x - 500) * 0.02f;
        float moveY = (newCursor.y - 500) * 0.02f;

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