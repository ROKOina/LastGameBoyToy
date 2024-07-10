#pragma once

#include <string>

template <class T>
class State
{
public:
    State(T* owner) :owner(owner) {}

    // ���̃X�e�[�g�ɑJ�ڂ���Ƃ��Ɉ�x�����Ă΂��
    virtual void Enter() {};

    // ���̃X�e�[�g�ł���ꍇ�A���t���[���Ă΂��
    virtual void Execute(const float& elapsedTime) {};

    // ���̃X�e�[�g���瑼�̃X�e�[�g�ɑJ�ڂ���Ƃ��Ɉ�x�����Ă΂��
    virtual void Exit() {};

    virtual void ImGui() {}

    std::string GetName() { return name; }

protected:
    std::string name = "";
    T* owner = nullptr;
};