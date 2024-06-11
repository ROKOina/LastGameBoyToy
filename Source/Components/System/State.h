#pragma once

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

protected:
    T* owner = nullptr;
};