#pragma once

#include "Components/System/Component.h"

class EasingMoveCom :public Component
{
public:

    EasingMoveCom(const char* filename);
    ~EasingMoveCom() {};

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "EasingMove"; }

private:

    //�C�[�W���O��~
    void StopEasing();

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
        float timescale = 0.0f;
        int easingtype = 0;
        int easingmovetype = 0;
        bool loop = false;
        bool comback = false;
        DirectX::XMFLOAT3 easingposition = {};
        DirectX::XMFLOAT3 easingscale = {};
        std::string objectname = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    EasingMoveParameter EMP = {};

private:

    float easingresult = 0.0f;
    float easingtime = 0.0f;
    bool play = false;
    bool loop = false;
    bool one = false;
    DirectX::XMFLOAT3 savepos = {};
    DirectX::XMFLOAT3 savescale = {};
};