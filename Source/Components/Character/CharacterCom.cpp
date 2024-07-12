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
    cameraForward.y = 0;

    GetGameObject()->transform_->SetRotation(QuaternionStruct::LookRotation(cameraForward).dxFloat4);
    GetGameObject()->transform_->UpdateTransform();
    GetGameObject()->transform_->SetUpTransform({ 0,1,0 });

    attackStateMachine.Update(elapsedTime);
    if (useMoveFlag)moveStateMachine.Update(elapsedTime);

#ifdef _DEBUG

    int inputNum = GetButtonDown();

    //�f�o�b�O����2�̃{�^�����������ōU���i��ʌ��Â炭�Ȃ�̖h�~�p
    if (CharacterInput::MainAttackButton & GetButtonDown()
        && GamePad::BTN_LEFT_SHOULDER & GetButton())
    {
        MainAttack();
    }

    ////�f�o�b�O����2�̃{�^�����������ōU���i��ʌ��Â炭�Ȃ�̖h�~�p
    //if (CharacterInput::SubAttackButton & GetButtonDown()
    //    && GamePad::BTN_RIGHT_SHOULDER & GetButton())
    //{
    //    SubAttack();
    //}

    if (CharacterInput::SubAttackButton & GetButtonDown())
    {
        SubAttack();
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
    moveStateMachine.ImGui();
    attackStateMachine.ImGui();
}

//�����A�j���[�V����
void CharacterCom::DirectionAnimation(std::weak_ptr<AnimationCom>animationCom, const DirectX::XMFLOAT3& movevec, const std::string& forward, const std::string& back, const std::string& right, const std::string& left, bool loop, const float& blendrate)
{
    //�O�㔻��
    float m_dotz = Mathf::Dot(GetGameObject()->transform_->GetWorldFront(), movevec);

    //�O�ς�Y�����ō��E����
    float m_crossy = Mathf::Cross(GetGameObject()->transform_->GetWorldFront(), movevec).y;

    ////�X�e�b�N�̓��͉����ŕς��A�j���[�V�����̑��x���ς��
    //if (speed)
    //{
    //    m_model->m_animation.AnimationSpeed(m_sticklength);
    //}

    //�O����
    if (m_dotz > 0 && fabs(m_crossy) < fabs(m_dotz) && animationCom.lock()->GetCurrentLowerAnimationIndex() != animationCom.lock()->FindAnimation(forward.c_str()))
    {
        animationCom.lock()->PlayLowerBodyOnlyAnimation(animationCom.lock()->FindAnimation(forward.c_str()), loop, false, blendrate);
    }
    //������
    else if (m_dotz < 0 && fabs(m_crossy) < fabs(m_dotz) && animationCom.lock()->GetCurrentLowerAnimationIndex() != animationCom.lock()->FindAnimation(back.c_str()))
    {
        animationCom.lock()->PlayLowerBodyOnlyAnimation(animationCom.lock()->FindAnimation(back.c_str()), loop, false, blendrate);
    }
    //�E����
    else if (m_crossy > 0 && fabs(m_crossy) >= fabs(m_dotz) && animationCom.lock()->GetCurrentLowerAnimationIndex() != animationCom.lock()->FindAnimation(right.c_str()))
    {
        animationCom.lock()->PlayLowerBodyOnlyAnimation(animationCom.lock()->FindAnimation(right.c_str()), loop, false, blendrate);
    }
    //������
    else if (m_crossy < 0 && fabs(m_crossy) >= fabs(m_dotz) && animationCom.lock()->GetCurrentLowerAnimationIndex() != animationCom.lock()->FindAnimation(left.c_str()))
    {
        animationCom.lock()->PlayLowerBodyOnlyAnimation(animationCom.lock()->FindAnimation(left.c_str()), loop, false, blendrate);
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

        std::shared_ptr<CameraCom> camera = cameraObj->GetComponent<CameraCom>();

        POINT cursor;
        ::GetCursorPos(&cursor);

        DirectX::XMFLOAT2 newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

        ::SetCursorPos(500, 500);

        float moveX = (newCursor.x - 500) * 0.02f;
        float moveY = (newCursor.y - 500) * 0.02f;

        //std::shared_ptr<GameObject> cameraPost = GameObjectManager::Instance().Find("cameraPostPlayer");

        //��]
        DirectX::XMFLOAT3 euler = GetGameObject()->transform_->GetEulerRotation();
        euler.y += moveX * 8.0f;
        //euler.x += moveY * 5.0f;

        //��]����
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
        //�L�����J�����ɐ؂�ւ�
        if (GamePad::BTN_LCONTROL & gamePad.GetButtonDown())
        {
            cameraObj->GetComponent<CameraCom>()->ActiveCameraChange();
            return;
        }
    }
}