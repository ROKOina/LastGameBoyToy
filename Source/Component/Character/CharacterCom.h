#pragma once
#include "../System\Component.h"
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Math\Mathf.h"
#include "Input\Input.h"
#include "Component\Animation\AimIKCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include <array>

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
    enum class CHARACTER_ULT {
        ATTACK,
        HEAL,
        POWER,
        MAX,
    };

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
        ULT,
        NONE,
        MAX,
    };

    //状態異常の種類
    enum class AbnormalCondition
    {
        STAN,
        MAX
    };

public:
    CharacterCom() {};
    ~CharacterCom() override {};

    // 名前取得
    const char* GetName() const override { return "Character"; }

    // 開始処理
    void Start() override {  };

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    //左クリック
    virtual void MainAttackDown() {};
    virtual void MainAttackPushing() {};
    //右クリック
    virtual void SubAttackDown() {};
    virtual void SubAttackPushing() {};

    //Q
    virtual void MainSkill() {};
    //E
    virtual void SubSkill() {};
    //R
    virtual void UltSkill() {};

    //LeftShift (固定ダッシュ)
    void LeftShiftSkill(float elapsedTime);

    virtual void SpaceSkill() {}

    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS>& GetAttackStateMachine() { return attackStateMachine; }
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS>& GetMoveStateMachine() { return moveStateMachine; }
    GameObject* GetCameraObj() { return cameraObj; }
    void SetCameraObj(GameObject* obj) { cameraObj = obj; }
    float GetJumpPower() { return jumpPower; }

    void SetStanSeconds(float sec) { stanTimer = sec; }

    //ネット側で決める
    void SetNetID(int id) { netID = id; }
    int GetNetID() { return netID; }
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
    float GetStickAngle() { return nowAngle; }
    void  SetStickAngle(const float angle) { stickAngle = angle; }

    DirectX::XMFLOAT3 GetFpsCameraDir() { return fpsCameraDir; }
    void  SetFpsCameraDir(const DirectX::XMFLOAT3 dir) { fpsCameraDir = dir; }

    int GetCharaID() { return charaID; }
    void  SetCharaID(const int id) { charaID = id; }

    void SetQSkillCoolTime(float time) { Qcool.time = time; }
    float GetQSkillCoolTime() { return Qcool.time; }
    void SetESkillCoolTime(float time) { Ecool.time = time; }
    float GetESkillCoolTime() { return Ecool.time; }
    void SetRSkillCoolTime(float time) { Rcool.time = time; }
    float GetRSkillCoolTime() { return Rcool.time; }
    void SetSpaceSkillCoolTime(float time) { Spacecool.time = time; }
    float GetSpaceSkillCoolTime() { return Spacecool.time; }

    void SetMoveFlag(bool flag) { useMoveFlag = flag; }

    bool* GetIsHitAttack() { return &isHitAttack; }
    void SetIsHitAttack(bool flg) { isHitAttack = flg; }

    void SetULTID(CHARACTER_ULT ult) { ultID = ult; }

private:
    //カメラ操作
    void CameraControl();

    //スタン更新
    void StanUpdate(float elapsedTime);

    //クールダウン更新
    void CoolUpdate(float elapsedTime);

    void SetLSSkillCoolTime(float time) { LScool.time = time; }
    float GetLSSkillCoolTime() { return LScool.time; }

    //アニメーションに使用する角度の補完
    float InterpolateAngle(float currentAngle, float targetAngle, float deltaTime, float speed);
    float Lerp(float start, float end, float t);

protected:
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS> moveStateMachine;
    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS> attackStateMachine;
    GameObject* cameraObj = nullptr;

    bool useMoveFlag = true;//falseにするとmoveStateを使わない
    float jumpPower = 3.0f;

    bool isStan = false;
    float stanTimer = 0;

    int teamID = 0;   //自分のチーム
    int netID = 0;//どのクライアントがこのキャラを担当するか
    std::array<float, 6> giveDamage = { 0,0,0,0,0,0 };//敵に与えたダメージ量や味方に与えた回復

    //スキルクールダウン
    struct SkillCoolTime
    {
        float time = 0;
        float timer = 100;
    };
    SkillCoolTime Qcool;
    SkillCoolTime Ecool;
    SkillCoolTime Rcool;
    SkillCoolTime LScool;
    SkillCoolTime Spacecool;

  CHARACTER_ULT ultID = CHARACTER_ULT::ATTACK;  //ウルトの種類　0:attack 1:heal 2:power

private:

    // キャラクターの操作入力情報
    unsigned int userInput = 0x00;
    unsigned int userInputDown = 0x00;
    unsigned int userInputUp = 0x00;
    DirectX::XMFLOAT2 leftStick = {};
    DirectX::XMFLOAT2 rightStick = {};

    //ダッシュ関係
    bool dashFlag = false;

    float dashGauge = 10;
    float dashGaugeMax = 10;
    float dashGaugeMinus = 1;   //一秒間減る数
    float dashGaugePlus = 2;

    float dashSpeed = 5;
    float dashSpeedFirst = 50;  //初速
    float dashSpeedNormal = 5; //ダッシュの速さ

    float dashRecast = 3;

    float dashFirstTimer = 0;
    float dashFirstTime = 0.1f; //初速長さ

    //ウルト関係
    bool isMaxUlt = false;  //ウルトが溜まっているか
    bool isUseUlt = false;  //ウルト使用中か
    float ultGauge = 0;
    float ultGaugeMax = 100;
    //アタックULT
    int attackUltCountMax = 5;  //ウルトを打てる数
    int attackUltCounter;

    AbnormalCondition abnormalcondition;

    //ネットに送る用のカメラの向き
    DirectX::XMFLOAT3 fpsCameraDir;

    int charaID;    //キャラクター識別用

    //野村追加
    float stickAngle = 0.0f;
    float nowAngle = 0.0f;
    float lerpSpeed = 4.0f;

    bool isHitAttack = false;   //攻撃が当たったフレーム時にtrue
};
