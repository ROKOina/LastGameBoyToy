#pragma once
#include "State.h"
#include <vector>
#include <memory>
#include <map>
#include <imgui.h>

template <class Owner, typename Enum>
class StateMachine
{
    using StatePtr = std::shared_ptr<State<Owner>>;

public:
    StateMachine() = default;
    // �R�s�[�R���X�g���N�^�h�~
    StateMachine(StateMachine const&) = delete;
    StateMachine& operator=(StateMachine const&) = delete;

    ~StateMachine() {
        stateList.clear();
    }

    void ImGui()
    {
        // �R���|�[�l���g
        for (auto& state : stateList)
        {
            if (state.second->GetName().empty())continue;

            ImGui::Spacing();
            ImGui::Separator();

            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
            if (ImGui::TreeNodeEx(state.second->GetName().c_str(), nodeFlags))
            {
                state.second->ImGui();
                ImGui::TreePop();
            }
        }
    }

    void AddState(Enum index, StatePtr state) {
        stateList.emplace(index, state);
    }

    void Update(const float& elapsedTime) {
        if (currentState != nullptr)
        {
            oldIndex = currentIndex;

            //assert(!currentState && !"�X�e�[�g���܂��o�^����Ă��܂���");
            currentState->Execute(elapsedTime);
        }
    }

    void ChangeState(Enum index) {
        // ���݂̃X�e�[�g���I��������
        if (currentState != nullptr)
        {
            nextIndex = index;
            currentState->Exit();
        }
        // �V�����X�e�[�g�ɐ؂�ւ��A�Ăяo��
        oldIndex = currentIndex;
        currentIndex = index;
        if (static_cast<int>(index) < 0)
        {
            currentState = nullptr;
            return;
        }

        currentState = stateList[index];
        currentState->Enter();
    }

    bool CurrentStateImGui(Enum index)
    {
        if (stateList.count(index) != 0)
        {
            stateList[index]->ImGui();
            return true;
        }
        return false;
    }

    Enum GetCurrentState() { return currentIndex; }
    Enum GetNextState() { return nextIndex; }
    Enum GetOldState() { return oldIndex; }

    StatePtr& GetState(Enum e) { return stateList[e]; }

private:
    StatePtr currentState = nullptr;
    Enum nextIndex;
    Enum currentIndex;
    Enum oldIndex;

    std::map<Enum, StatePtr> stateList;
};


