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

//�v���C���[�p�L�[���͕⏕�N���X
class CharacterInput
{
public:
    static constexpr GamePadButton JumpButton_SPACE = GamePad::BTN_A;
    static constexpr GamePadButton LeftShiftButton = GamePad::BTN_LEFT_SHOULDER;
    static constexpr GamePadButton MainSkillButton_Q = GamePad::BTN_B;
    static constexpr GamePadButton SubSkillButton_E = GamePad::BTN_X;
    static constexpr GamePadButton UltimetButton_R = GamePad::BTN_Y;
    static constexpr GamePadButton MainAttackButton = GamePad::BTN_RIGHT_TRIGGER;   //�}�E�X��
    static constexpr GamePadButton SubAttackButton = GamePad::BTN_LEFT_TRIGGER;     //�}�E�X�E
};

#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeMoveState(State) charaCom.lock()->GetMoveStateMachine().ChangeState(State);
#define ChangeAttackState(State) charaCom.lock()->GetAttackStateMachine().ChangeState(State);

//�g�p�X�L���̃^�O
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

    //�U����X�L�����g�������̋���
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

    // ���O�擾
    const char* GetName() const override { return "Character"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    //���N���b�N
    virtual void MainAttackDown() {};
    virtual void MainAttackPushing() {};
    //�E�N���b�N
    virtual void SubAttackDown() {};
    virtual void SubAttackPushing() {};

    //Q
    virtual void MainSkill() {};
    //E
    virtual void SubSkill() {};
    //R
    virtual void UltSkill() {};

    //LeftShift (�Œ�_�b�V��)
    void DashFewSub(float elapsedTime);

    //�X�y�[�X�X�L��(�オ�P�������ŉ���Pushing��������)
    virtual void SpaceSkill() {}
    virtual void SpaceSkillPushing(float elapsedTime) {};

    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS>& GetAttackStateMachine() { return attackStateMachine; }
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS>& GetMoveStateMachine() { return moveStateMachine; }
    GameObject* GetCameraObj() { return cameraObj; }
    void SetCameraObj(GameObject* obj) { cameraObj = obj; }
    float GetJumpPower() { return jumpPower; }

    void SetStanSeconds(float sec) { stanTimer = sec; }

    // ������͏��
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

    //�g�p�X�L���o�^
    void SetUseSkill(USE_SKILL use) { myUseSkill = use; }
    USE_SKILL GetUseSkill() { return myUseSkill; }

    //�X�L���N�[���_�E���n
    enum SkillCoolID
    {
        Q, E, R, LeftShift, Space, LeftClick, MAX
    };
    void SetSkillCoolTime(SkillCoolID id, float time) { skillCools[id].time = time; }
    float GetSkillCoolTime(SkillCoolID id) { return skillCools[id].time; }
    float* GetSkillCoolTimerPointer(SkillCoolID id) { return &skillCools[id].timer; }
    void ResetSkillCoolTimer(SkillCoolID id) { skillCools[id].timer = skillCools[id].time; }    //�}�b�N�X�̏�Ԃɂ���
    bool IsSkillCoolMax(SkillCoolID id) { return skillCools[id].timer >= skillCools[id].time; }

    //�l�b�g�֘A�ϐ��Q�b�^�[
    NetCharaData& GetNetCharaData() { return netCharaData; }

private:
    //���̓X�e�[�g�X�V
    void InputStateUpdate(float elapsedTime);

    //�J��������
    void CameraControl();

    //�_�b�V���֌W�i�_�b�V�������Ԃ��j
    bool DashUpdateReIsDash(float elapsedTime);

    //�r�l�b�g����
    void Vinetto(float elapsedTime);

    //�X�^���X�V
    void StanUpdate(float elapsedTime);

    //�N�[���_�E���X�V
    void CoolUpdate(float elapsedTime);

    //�A�j���[�V�����Ɏg�p����p�x�̕⊮
    float InterpolateAngle(float currentAngle, float targetAngle, float deltaTime, float speed);
    float Lerp(float start, float end, float t);

    //UltUpdate
    void UltUpdate(float elapsedTime);

protected:
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS>   moveStateMachine;
    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS> attackStateMachine;
    GameObject* cameraObj = nullptr;    //�����̃L�����̏ꍇ��������

    bool useMoveFlag = true;//false�ɂ����moveState���g��Ȃ�
    float jumpPower = 3.0f;

    bool isStan = false;
    float stanTimer = 0;

    //�X�L���N�[���_�E��
    struct SkillCoolTime
    {
        float time = 0;
        float timer = 100;
    };
    SkillCoolTime skillCools[SkillCoolID::MAX];

    //�g�p�X�L��
    USE_SKILL myUseSkill = USE_SKILL::NONE;

protected:

    bool boostflag = false;
    float dashGauge = 10;

private:

    // �L�����N�^�[�̑�����͏��
    unsigned int userInput = 0x00;
    unsigned int userInputDown = 0x00;
    unsigned int userInputUp = 0x00;
    DirectX::XMFLOAT2 leftStick = {};
    DirectX::XMFLOAT2 rightStick = {};

    //�_�b�V���֌W
    bool dashFlag = false;
    float dashGaugeMinus = 1;   //��b�Ԍ��鐔
    float dashGaugeMax = 10;
    float dashGaugePlus = 2;

    float dashSpeed = 58;
    float dashSpeedFirst = 58;  //����
    float dashSpeedNormal = 13; //�_�b�V���̑���

    //�_�b�V�������r�؂�Ȃ��悤��
    float dashDraceTime = 0.1f;
    float dashDraceTimer = 0;

    float dashRecast = 3;

    float dashFirstTimer = 0;
    float dashFirstTime = 0.1f; //��������

    //�E���g�֌W
    bool isMaxUlt = false;  //�E���g�����܂��Ă��邩
    bool isUseUlt = false;  //�E���g�g�p����
    bool prevIsMaxUlt = false;
    float ultGauge = 0;
    float ultGaugeMax = 100;

    //�l�b�g�ɑ���p�̃J�����̌���
    DirectX::XMFLOAT3 fpsCameraDir;

    //�쑺�ǉ�
    float stickAngle = 0.0f;
    float nowAngle = 0.0f;
    float lerpSpeed = 4.0f;

    bool isHitAttack = false;   //�U�������������t���[������true

    NetCharaData netCharaData;  //�l�b�g�֘A
};
