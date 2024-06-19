#pragma once
#include "../System\Component.h"
#include "../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"
#include "Input\Input.h"

//プレイヤー用キー入力補助クラス
class CharacterInput
{
public:
    static constexpr GamePadButton JumpButton_SPACE = GamePad::BTN_A;
    static constexpr GamePadButton MainSkillButton_Q = GamePad::BTN_LEFT_SHOULDER;
    static constexpr GamePadButton SubSkillButton_E = GamePad::BTN_RIGHT_SHOULDER;
    static constexpr GamePadButton UltimetButton_R = GamePad::BTN_Y;
    static constexpr GamePadButton MainAttackButton = GamePad::BTN_RIGHT_TRIGGER;   //マウス左
    static constexpr GamePadButton SubAttackButton = GamePad::BTN_LEFT_TRIGGER;     //マウス右
};

class CharacterCom : public Component
{
public:
    enum class CHARACTER_ACTIONS {
        IDLE,
        MOVE,
        DASH,
        JUMP,
        ATTACK,
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
    virtual void UltSkill() {};

    virtual void SpaceSkill() {}

    StateMachine<CharacterCom, CHARACTER_ACTIONS>& GetStateMachine() { return stateMachine; }
    float GetJumpPower() { return jumpPower; }

private:
    //カメラ操作
    void CameraControl();

protected:
    StateMachine<CharacterCom, CHARACTER_ACTIONS> stateMachine;

    float jumpPower = 5.0f;
};
