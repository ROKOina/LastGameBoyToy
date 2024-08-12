#pragma once

#include <string>

template <class T>
class State
{
public:
    State(T* owner) :owner(owner) {}

    // このステートに遷移するときに一度だけ呼ばれる
    virtual void Enter() {};

    // このステートである場合、毎フレーム呼ばれる
    virtual void Execute(const float& elapsedTime) {};

    // このステートから他のステートに遷移するときに一度だけ呼ばれる
    virtual void Exit() {};

    //imgui
    virtual void ImGui() {}

    // 名前取得
    virtual const char* GetName() const = 0;

protected:
    T* owner = nullptr;
};