#include "CharacterCom.h"
#include "MovementCom.h"
#include "CameraCom.h"
#include "GameSource\Scene\SceneManager.h"
#include "Input\Input.h"

void CharacterCom::Update(float elapsedTime)
{
    GamePad gamePad = Input::Instance().GetGamePad();
    MovementCom* moveCom = GetGameObject()->GetComponent<MovementCom>().get();

    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = CalcMoveVec();

    //�ړ��x�N�g����ݒ�
    //DirectX::XMFLOAT3 v = moveVec * moveCom->GetMoveAcceleration();
    //moveCom->SetVelocityX(v.x);
    //moveCom->SetVelocityZ(v.z);
}

DirectX::XMFLOAT3 CharacterCom::CalcMoveVec() const
{
    std::shared_ptr<GameObject> cameraObj = SceneManager::Instance().GetActiveCamera();
    CameraCom* cameraCom = cameraObj->GetComponent<CameraCom>().get();

    //���͏����擾
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    //�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    const DirectX::XMFLOAT3& cameraRight = cameraCom->GetRight();
    const DirectX::XMFLOAT3& cameraFront = cameraCom->GetFront();

    //�ړ��x�N�g����XZ����

    //�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        //�P�ʃx�N�g����
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    //�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        //�P�ʃx�N�g����
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    //�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
    //�X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
    //�i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec = {};
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    vec.y = 0.0f;

    return vec;
}
