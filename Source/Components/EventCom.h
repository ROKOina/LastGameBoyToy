#pragma once

#include "Components/System/Component.h"

class EventCom :public Component
{
public:

    EventCom(const char* filename);
    ~EventCom() {};

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Event"; }

public:

    // �C�x���g�̃p�����[�^���i�[����\����
    struct EventParameter
    {
        float starttime = 0.0f;        // �J�n����
        float totaltime = 0.0f;        // �o�ߎ���
        float endtime = 0.0f;          // �I������
        std::string eventname = "New Event";
        bool active = false;           // �C�x���g�̗L��/�������
        bool finished = false;         // �C�x���g�̊������
        bool loop = false;             // ���[�v�t���O
    };
    std::vector<EventParameter> eventList;
};