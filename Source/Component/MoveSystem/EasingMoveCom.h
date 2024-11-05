#pragma once

#include "Component/System/Component.h"

class EasingMoveCom :public Component
{
public:

    EasingMoveCom(const char* filename);
    ~EasingMoveCom() {};

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "EasingMove"; }

    // �I�����̃R�[���o�b�N�֐�
    std::function<void()> onFinishCallback = nullptr;

private:

    //�C�[�W���O��~
    void StopEasing();

    //�I�u�W�F�N�g�������
    void Object();

    //�V���A���C�Y
    void Serialize();

    // �f�V���A���C�Y
    void Deserialize(const char* filename);

    // �f�V���A���C�Y�̓ǂݍ���
    void LoadDeserialize();

public:

    struct EasingMoveParameter
    {
        std::string	filename = {};
        float delaytime = {};
        bool delatimeuse = false;
        std::vector<float> timescale = {};
        std::vector<int> easingtype = {};
        std::vector<int> easingmovetype = {};
        std::vector<DirectX::XMFLOAT3> easingposition = {};
        std::string objectname = {};
        float trackingtime = 0.0f;
        bool deleteflag = false;
        float deletetime = 0.0f;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    EasingMoveParameter EMP = {};

private:

    float easingresult = 0.0f;
    float easingtime = 0.0f;
    float time = 0.0f;
    float deletetime = 0.0f;
    bool play = false;
    bool stop = false;
    DirectX::XMFLOAT3 savepos = {};
    size_t currentTargetIndex = 0; // ���݂̖ڕW�|�C���g�̃C���f�b�N�X
    std::string filepath = {};
};