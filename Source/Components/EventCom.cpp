#include "EventCom.h"

//������
EventCom::EventCom(const char* filename)
{
}

//�����ݒ�
void EventCom::Start()
{
}

//�X�V����
void EventCom::Update(float elapsedTime)
{
    for (auto& event : eventList)
    {
        if (event.active && !event.finished)
        {
            // �J�n�O�Ȃ�X�V���Ȃ�
            if (event.totaltime < event.starttime) continue;

            // �C�x���g�i�s��
            event.totaltime += elapsedTime;
            for (auto& event : eventList)
            {
                if (!event.active)continue;// �C�x���g�������܂��͏I�����Ă���ꍇ�͍X�V���Ȃ�

                if (event.totaltime < event.starttime)continue;// �J�n�O�Ȃ��Ԃ��X�V���A���̃C�x���g��

                // �C�x���g�i�s��
                event.totaltime += elapsedTime;

                // �C�x���g�I������
                if (event.totaltime >= event.endtime)
                {
                    if (event.loop)  // ���[�v�Ȃ烊�Z�b�g
                    {
                        event.totaltime = event.starttime;
                    }
                    else
                    {
                        event.totaltime = event.endtime;  // ������Ԃ̏ꍇ�A���v���Ԃ��I�����Ԃɐݒ�
                    }
                }
            }

            // �C�x���g�I������
            if (event.totaltime >= event.endtime)
            {
                if (event.loop)  // ���[�v�Ȃ烊�Z�b�g
                {
                    event.totaltime = event.starttime;
                }
                else
                {
                    event.finished = true;
                }
            }
        }
    }
}

//imgui
void EventCom::OnGUI()
{
    for (int i = 0; i < eventList.size(); ++i)
    {
        EventParameter& event = eventList[i];
        std::string eventID = "##Event" + std::to_string(i);

        // �C�x���g���\��
        ImGui::Text(event.eventname.c_str(), i + 1);

        // �C�x���g���ҏW
        char eventNameBuffer[256];
        strncpy_s(eventNameBuffer, sizeof(eventNameBuffer), event.eventname.c_str(), _TRUNCATE);
        if (ImGui::InputText(("Event Name" + eventID).c_str(), eventNameBuffer, sizeof(eventNameBuffer)))
        {
            event.eventname = eventNameBuffer;
        }

        // ���ԃp�����[�^�̕ҏW
        ImGui::InputFloat(("Start Time" + eventID).c_str(), &event.starttime);
        ImGui::InputFloat(("End Time" + eventID).c_str(), &event.endtime);
        ImGui::InputFloat(("Total Time" + eventID).c_str(), &event.totaltime);

        // �C�x���g�̏��
        ImGui::Checkbox(("Active" + eventID).c_str(), &event.active);
        ImGui::SameLine();
        ImGui::Checkbox(("Loop" + eventID).c_str(), &event.loop);
        ImGui::SameLine();
        ImGui::Checkbox(("Finish" + eventID).c_str(), &event.finished);

        // �C�x���g�̏�Ԃ�\��
        if (event.finished)
        {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: Finished");
        }
        else if (event.totaltime >= event.starttime && event.totaltime < event.endtime)
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Status: In Progress");
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: Not Started");
        }

        // �폜�{�^��
        if (ImGui::Button(("Delete" + eventID).c_str()))
        {
            eventList.erase(eventList.begin() + i);
            i--;  // �C�x���g���폜���ꂽ���߁A�C���f�b�N�X�𒲐�
        }

        ImGui::SameLine();

        // �R�s�[���y�[�X�g�{�^��
        if (ImGui::Button(("Copy" + eventID).c_str()))
        {
            eventList.push_back(event);  // �C�x���g���R�s�[
        }

        ImGui::Separator();
    }

    // �C�x���g�̒ǉ�
    if (ImGui::Button("Add Event"))
    {
        EventParameter newEvent;
        newEvent.eventname = "New Event";
        eventList.push_back(newEvent);
    }
}