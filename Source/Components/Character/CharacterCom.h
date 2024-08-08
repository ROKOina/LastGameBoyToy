#pragma once
#include "../System\Component.h"
#include "../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"
#include "Input\Input.h"
#include "../AnimationCom.h"
#include <array>

#include "../MovementCom.h"

//プレイヤー用キー入力補助クラス
class CharacterInput
{
public:
    static constexpr GamePadButton JumpButton_SPACE = GamePad::BTN_A;
    static constexpr GamePadButton LeftShiftButton = GamePad::BTN_LEFT_SHOULDER;
    static constexpr GamePadButton MainSkillButton_Q = GamePad::BTN_B;
    static constexpr GamePadButton SubSkillButton_E = GamePad::BTN_X;
    static constexpr GamePadButton UltimetButton_R = GamePad::BTN_Y;
    static constexpr GamePadButton MainAttackButton = GamePad::BTN_RIGHT_TRIGGER;   //マウス左
    static constexpr GamePadButton SubAttackButton = GamePad::BTN_LEFT_TRIGGER;     //マウス右
};

#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeMoveState(State) charaCom.lock()->GetMoveStateMachine().ChangeState(State);
#define ChangeAttackState(State) charaCom.lock()->GetAttackStateMachine().ChangeState(State);

class CharacterCom : public Component
{
public:
    enum class CHARACTER_MOVE_ACTIONS {
        IDLE,
        MOVE,
        DASH,
        JUMP,
        JUMPLOOP,
        LANDING,
        NONE,
        MAX,
    };

    //攻撃やスキルを使った時の挙動
    enum class CHARACTER_ATTACK_ACTIONS {
        PANTCH,
        RELOAD,
        MAIN_ATTACK,
        SUB_ATTACK,
        MAIN_SKILL,
        SUB_SKILL,
        NONE,
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

    //左クリック
    virtual void MainAttack() {};
    //右クリック
    virtual void SubAttack() {};

    //Q
    virtual void MainSkill() {};
    //E
    virtual void SubSkill() {};
    //R
    virtual void UltSkill() {};
    //LeftShift
    virtual void LeftShiftSkill() {};

    virtual void SpaceSkill() {}

    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS>& GetAttackStateMachine() { return attackStateMachine; }
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS>& GetMoveStateMachine() { return moveStateMachine; }
    GameObject* GetCameraObj() { return cameraObj; }
    void SetCameraObj(GameObject* obj) { cameraObj = obj; }
    float GetJumpPower() { return jumpPower; }

    void SetHitPoint(float value) { hitPoint = value; }
    float GetHitPoint() { return hitPoint; }
    void AddHitPoint(float value) { hitPoint += value; }

    void SetMoveMaxSpeed(float speed)
    {
        GetGameObject()->GetComponent<MovementCom>()->SetMoveMaxSpeed(speed);
    }
    float GetMoveMaxSpeed()
    {
        return GetGameObject()->GetComponent<MovementCom>()->GetMoveMaxSpeed();
    }

    //ネット側で決める
    void SetCharaID(int id) { charaID = id; }
    int GetCharaID() { return charaID; }
    //ネット側で決める
    void SetTeamID(int id) { teamID = id; }
    int GetTeamID() { return teamID; }
    void AddGiveDamage(int index, float damage) { giveDamage[index] += damage; }
    std::array<float, 6> GetGiveDamage() { return giveDamage; }

    // 操作入力情報
    void SetUserInput(const GamePadButton& button) { userInput = button; }
    void SetUserInputDown(const GamePadButton& button) { userInputDown = button; }
    void SetUserInputUp(const GamePadButton& button) { userInputUp = button; }
    GamePadButton GetButton() { return userInput; }
    GamePadButton GetButtonDown() { return userInputDown; }
    GamePadButton GetButtonUp() { return userInputUp; }

    void SetLeftStick(const  DirectX::XMFLOAT2& stick) { leftStick = stick; }
    void SetRightStick(const DirectX::XMFLOAT2& stick) { rightStick = stick; }
    DirectX::XMFLOAT2 GetLeftStick() { return leftStick; }
    DirectX::XMFLOAT2 GetRightStick() { return rightStick; }
    bool IsPushLeftStick() {
        if (leftStick.x * leftStick.x + leftStick.y * leftStick.y > 0)
            return true;

        return false;
    }
    float GetStickAngle() { return stickAngle; }
    void  SetStickAngle(const float angle) { stickAngle = angle; }

private:
    //カメラ操作
    void CameraControl();

protected:
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS> moveStateMachine;
    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS> attackStateMachine;
    GameObject* cameraObj = nullptr;

    bool useMoveFlag = true;//falseにするとmoveStateを使わない
    float jumpPower = 1.0f;
    float hitPoint = 100.0f;

    int teamID = 0;   //自分のチーム
    int charaID = 0;//どのクライアントがこのキャラを担当するか
    std::array<float, 6> giveDamage = { 0,0,0,0,0,0 };//敵に与えたダメージ量や味方に与えた回復

private:

    // キャラクターの操作入力情報
    unsigned int userInput = 0x00;
    unsigned int userInputDown = 0x00;
    unsigned int userInputUp = 0x00;
    DirectX::XMFLOAT2 leftStick = {};
    DirectX::XMFLOAT2 rightStick = {};
    //野村追加
    float stickAngle = 0.0f;
};
