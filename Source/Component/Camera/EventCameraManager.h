#pragma once

#include <string>
#include <vector>
#include <memory>
#include <DirectXMath.h>

class GameObject;

// �C�x���g�J�����̓�����ۑ�
class EventCameraManager
{
public:
    EventCameraManager() {}
    ~EventCameraManager() {}

    // �C���X�^���X�擾
    static EventCameraManager& Instance()
    {
        static EventCameraManager instance;
        return instance;
    }

private:
    //�V���A���C�Y���񂯂�
    void Serialize();
    void Deserialize(const char* filename);
    void LoadDesirialize();

public:

    void EventCameraImGui();
    void EventUpdate(float elapsedTime);

    //�J�����̈ʒu���ۑ�
    struct ECTransform
    {
        float frame;            //�t���[��
        DirectX::XMFLOAT3 pos;  //�ʒu

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    //�V���A���C�U���
    struct SaveEventCameraBuff
    {
        std::string cameraName;     //�J�����I�u�W�F�̖��O
        std::string focusObjName;   //�����_�I�u�W�F�̖��O
        std::vector<ECTransform> ECTra;

        //ver2
        bool isFocusLocal = false;  //�����_�̎q�ɂ��邩

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

public:
    //�C�x���g�Đ�
    void PlayEventCamera(std::string eventName);

    bool GetIsPlayEvent() { return isEventPlay; }

private:
    //�t�H�[�J�X�|�W�V�������猩���J�����|�W�V������Ԃ�
    DirectX::XMFLOAT3 FocusFromCameraPos();

    //�t�H�[�J�X�|�W�V�������猩�����[���h�|�W�V������Ԃ�
    DirectX::XMFLOAT3 FocusFromWorldPos(DirectX::XMFLOAT3 pos);

    //�Đ��֐�(����)
    void PlayCameraLerp();

private:
    SaveEventCameraBuff saveEventCameraBuff;

    //�Q�[���I�u�W�F�N�g�ۑ��p
    std::weak_ptr<GameObject> cameraObj;
    std::weak_ptr<GameObject> focusObj;

    //�Đ��֌W
    bool isEventPlay = false;   //�Đ�����
    float timer;                //�Đ����̎���
};