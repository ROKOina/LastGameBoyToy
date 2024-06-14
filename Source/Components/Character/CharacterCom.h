#pragma once
#include "../System\Component.h"
#include "../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"

class CharacterCom : public Component
{
public:
    enum class CHARACTER_ACTIONS {
        IDLE,
        MOVE,
        JUMP,
        MAX,
    };

public:
    CharacterCom() {};
    ~CharacterCom() override {};

    // 名前取得
    const char* GetName() const override { return "Character"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    virtual void MainAttack() {};
    virtual void SubAttack() {};

    virtual void MainSkill() {};
    virtual void SubSkill() {};
    virtual void DashSkill() {};
    virtual void UltSkill() {};

    StateMachine<CharacterCom, CHARACTER_ACTIONS>& GetStateMachine() { return stateMachine; }
    float GetJumpPower() { return jumpPower; }

protected:
    StateMachine<CharacterCom, CHARACTER_ACTIONS> stateMachine;

    float jumpPower = 5.0f;
};
