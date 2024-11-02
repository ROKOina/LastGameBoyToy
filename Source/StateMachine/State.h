#pragma once

#include <string>

//�U���̎��
enum class AttackType
{
    NON,
    MELEE,
    BULLET,
    LAZER,
    HEAL,
    MAX
};

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

    //imgui
    virtual void ImGui() {}

    // ���O�擾
    virtual const char* GetName() const = 0;

    // AttackType�𕶎���ɕϊ�����֐�
    const char* GetAttackTypeName(AttackType type)
    {
        switch (type)
        {
        case AttackType::NON: return "Non";
        case AttackType::MELEE: return "Melee";
        case AttackType::BULLET: return "Bullet";
        case AttackType::LAZER: return "Lazer";
        case AttackType::HEAL: return "Heal";
        default: return "Unknown";
        }
    }
    AttackType attacktype = AttackType::NON;

protected:
    T* owner = nullptr;
};