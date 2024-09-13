#pragma once

#include <string>

//攻撃の種類
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

    // AttackTypeを文字列に変換する関数
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