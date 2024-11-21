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
    // コピーコンストラクタ防止
    StateMachine(StateMachine const&) = delete;
    StateMachine& operator=(StateMachine const&) = delete;

    ~StateMachine() {
        stateList.clear();
    }

    void ImGui()
    {
        ImGui::Text("Attack Type: %s", currentState->GetAttackTypeName(currentState->attacktype));

        if (currentState != nullptr)
        {
            ImGui::Text("Current State: %s", currentState->GetName());
        }
        else
        {
            ImGui::Text("Current State: None");
        }

        for (const auto& [key, state] : stateList)
        {
            //ここら辺のimguiは修正が必用
            if (ImGui::Button(state->GetName()))
            {
                ChangeState(key);
            }

            if (ImGui::TreeNode(state->GetName()))
            {
                state->ImGui();
                ImGui::TreePop();
            }
        }
    }

    void AddState(Enum index, StatePtr state, AttackType m_attacktype = AttackType::NON)
    {
        stateList.emplace(index, state);
        state->attacktype = m_attacktype;
    }

    void Update(const float& elapsedTime) {
        if (currentState != nullptr)
        {
            oldIndex = currentIndex;

            //assert(!currentState && !"ステートがまだ登録されていません");
            currentState->Execute(elapsedTime);
        }
    }

    void ChangeState(Enum index) {
        // 現在のステートを終了させる
        if (currentState != nullptr)
        {
            nextIndex = index;
            currentState->Exit();
        }
        // 新しいステートに切り替え、呼び出し
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

    // コンポーネント取得
    template<class T>
    std::shared_ptr<T> GetState()
    {
        for (auto& [key, state] : stateList)
        {
            std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(state);
            if (p == nullptr) continue;
            return p;
        }
        return nullptr;
    }


    StatePtr GetState(Enum e)
    {
        return stateList[e];
    }

    Enum GetCurrentState() { return currentIndex; }
    Enum GetNextState() { return nextIndex; }
    Enum GetOldState() { return oldIndex; }

private:
    StatePtr currentState = nullptr;
    Enum nextIndex;
    Enum currentIndex;
    Enum oldIndex;

    std::map<Enum, StatePtr> stateList;
};