#include "SceneManager.h"

#include "Components/System/GameObject.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Graphics/Graphics.h"
#include "Input\Input.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

//�X�V����
void SceneManager::Update(float elapsedTime)
{
    if (nextScene_ != nullptr)
    {
        //�Â��V�[�����I������
        Clear();

        //�V�����V�[����ݒ�
        currentScene_ = nextScene_;
        nextScene_ = nullptr;

        //�V�[������������
        if (!currentScene_->IsReady())currentScene_->Initialize();
    }

    if (currentScene_ != nullptr)
    {
        currentScene_->Update(elapsedTime);

        //�J�����`�F���W�����p
        isChangeCamera = false;
        if (cameraActiveCount >= 2)
        {
            isChangeCamera = true;
        }
        cameraActiveCount = 0;
    }
}

//�`�揈��
void SceneManager::Render(float elapsedTime)
{
    if (currentScene_ != nullptr)
    {
        currentScene_->Render(elapsedTime);
    }
}

//�V�[���N���A
void SceneManager::Clear()
{
    if (currentScene_ != nullptr)
    {
        currentScene_->Finalize();
        delete currentScene_;
        currentScene_ = nullptr;
    }
}

//�V�[���؂�ւ�
void SceneManager::ChangeScene(Scene* scene)
{
    //�V�����V�[����ݒ�
    nextScene_ = scene;
}

DirectX::XMFLOAT3 SceneManager::InputVec() const
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