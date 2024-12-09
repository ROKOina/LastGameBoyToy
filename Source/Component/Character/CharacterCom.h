#pragma once
#include "../System\Component.h"
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Math\Mathf.h"
#include "Input\Input.h"
#include "Component\Animation\AimIKCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Prop/NetCharaData.h"
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

//使用スキルのタグ
enum USE_SKILL : uint64_t
{
    NONE = 1 << 0,

    Q = 1 << 1,
    E = 1 << 2,
    RIGHT_CLICK = 1 << 3,
};
static USE_SKILL operator| (USE_SKILL L, USE_SKILL R)
{
    return static_cast<USE_SKILL>(static_cast<uint64_t>(L) | static_cast<uint64_t>(R));
}
static USE_SKILL operator& (USE_SKILL L, USE_SKILL R)
{
    return static_cast<USE_SKILL>(static_cast<uint64_t>(L) & static_cast<uint64_t>(R));
}
static bool operator== (USE_SKILL L, USE_SKILL R)
{
    if (static_cast<uint64_t>((static_cast<USE_SKILL>(L) & static_cast<USE_SKILL>(R))) == 0)
        return false;
    return true;
}
static bool operator!= (USE_SKILL L, USE_SKILL R)
{
    if (static_cast<uint64_t>((static_cast<USE_SKILL>(L) & static_cast<USE_SKILL>(R))) == 0)
        return true;
    return false;
}

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
        DEATH,
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
    void DashFewSub(float elapsedTime);

    //スペーススキル(上が単発押しで下のPushingが長押し)
    virtual void SpaceSkill() {}
    virtual void SpaceSkillPushing(float elapsedTime) {};

    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS>& GetAttackStateMachine() { return attackStateMachine; }
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS>& GetMoveStateMachine() { return moveStateMachine; }
    GameObject* GetCameraObj() { return cameraObj; }
    void SetCameraObj(GameObject* obj) { cameraObj = obj; }
    float GetJumpPower() { return jumpPower; }

    void SetStanSeconds(float sec) { stanTimer = sec; }

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


    void SetUltGauge(float gauge) { ultGauge = gauge; }
    float* GetUltGauge() { return  &ultGauge; }
    float GetUltGaugeMax() { return ultGaugeMax; }

    void SetMoveFlag(bool flag) { useMoveFlag = flag; }

    float GetDahsGaugeMax() { return dashGaugeMax; }
    float* GetDashGauge() { return &dashGauge; }

    bool* GetIsHitAttack() { return &isHitAttack; }
    void SetIsHitAttack(bool flg) { isHitAttack = flg; }

    bool UseUlt() { return isUseUlt; }
    void FinishUlt() { isUseUlt = false; }

    //使用スキル登録
    void SetUseSkill(USE_SKILL use) { myUseSkill = use; }
    USE_SKILL GetUseSkill() { return myUseSkill; }

    //スキルクールダウン系
    enum SkillCoolID
    {
        Q, E, R, LeftShift, Space, LeftClick, MAX
    };
    void SetSkillCoolTime(SkillCoolID id, float time) { skillCools[id].time = time; }
    float GetSkillCoolTime(SkillCoolID id) { return skillCools[id].time; }
    float* GetSkillCoolTimerPointer(SkillCoolID id) { return &skillCools[id].timer; }
    void ResetSkillCoolTimer(SkillCoolID id) { skillCools[id].timer = skillCools[id].time; }    //マックスの状態にする
    bool IsSkillCoolMax(SkillCoolID id) { return skillCools[id].timer >= skillCools[id].time; }

    //ネット関連変数ゲッター
    NetCharaData& GetNetCharaData() { return netCharaData; }

private:
    //入力ステート更新
    void InputStateUpdate(float elapsedTime);

    //カメラ操作
    void CameraControl();

    //ダッシュ関係（ダッシュ中か返す）
    bool DashUpdateReIsDash(float elapsedTime);

    //ビネット効果
    void Vinetto(float elapsedTime);

    //スタン更新
    void StanUpdate(float elapsedTime);

    //クールダウン更新
    void CoolUpdate(float elapsedTime);

    //アニメーションに使用する角度の補完
    float InterpolateAngle(float currentAngle, float targetAngle, float deltaTime, float speed);
    float Lerp(float start, float end, float t);

    //UltUpdate
    void UltUpdate(float elapsedTime);

protected:
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS>   moveStateMachine;
    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS> attackStateMachine;
    GameObject* cameraObj = nullptr;    //自分のキャラの場合だけ入る

    bool useMoveFlag = true;//falseにするとmoveStateを使わない
    float jumpPower = 3.0f;

    bool isStan = false;
    float stanTimer = 0;

    //スキルクールダウン
    struct SkillCoolTime
    {
        float time = 0;
        float timer = 100;
    };
    SkillCoolTime skillCools[SkillCoolID::MAX];

    //使用スキル
    USE_SKILL myUseSkill = USE_SKILL::NONE;

protected:

    bool boostflag = false;
    float dashGauge = 10;

private:

    // キャラクターの操作入力情報
    unsigned int userInput = 0x00;
    unsigned int userInputDown = 0x00;
    unsigned int userInputUp = 0x00;
    DirectX::XMFLOAT2 leftStick = {};
    DirectX::XMFLOAT2 rightStick = {};

    //ダッシュ関係
    bool dashFlag = false;
    float dashGaugeMinus = 1;   //一秒間減る数
    float dashGaugeMax = 10;
    float dashGaugePlus = 2;

    float dashSpeed = 58;
    float dashSpeedFirst = 58;  //初速
    float dashSpeedNormal = 13; //ダッシュの速さ

    //ダッシュすぐ途切れないように
    float dashDraceTime = 0.1f;
    float dashDraceTimer = 0;

    float dashRecast = 3;

    float dashFirstTimer = 0;
    float dashFirstTime = 0.1f; //初速長さ

    //ウルト関係
    bool isMaxUlt = false;  //ウルトが溜まっているか
    bool isUseUlt = false;  //ウルト使用中か
    bool prevIsMaxUlt = false;
    float ultGauge = 0;
    float ultGaugeMax = 100;

    //ネットに送る用のカメラの向き
    DirectX::XMFLOAT3 fpsCameraDir;

    //野村追加
    float stickAngle = 0.0f;
    float nowAngle = 0.0f;
    float lerpSpeed = 4.0f;

    bool isHitAttack = false;   //攻撃が当たったフレーム時にtrue

    NetCharaData netCharaData;  //ネット関連
};
