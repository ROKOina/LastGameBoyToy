#pragma once
#include "../System\Component.h"
#include "../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"
#include "Input\Input.h"
#include "../AnimationCom.h"
#include <array>

#include "../MovementCom.h"

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

    //�U����X�L�����g�������̋���
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

    //��Ԉُ�̎��
    enum class AbnormalCondition
    {
        STAN,
        MAX
    };

public:
    CharacterCom() {};
    ~CharacterCom() override {};

    // ���O�擾
    const char* GetName() const override { return "Character"; }

    // �J�n����
    void Start() override {  };

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
    void LeftShiftSkill();

    virtual void SpaceSkill() {}

    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS>& GetAttackStateMachine() { return attackStateMachine; }
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS>& GetMoveStateMachine() { return moveStateMachine; }
    GameObject* GetCameraObj() { return cameraObj; }
    void SetCameraObj(GameObject* obj) { cameraObj = obj; }
    float GetJumpPower() { return jumpPower; }

  void SetStanSeconds(float sec) { stanTimer = sec; }

  //�l�b�g���Ō��߂�
  void SetNetID(int id) { netID = id; }
  int GetNetID() { return netID; }
  //�l�b�g���Ō��߂�
  void SetTeamID(int id) { teamID = id; }
  int GetTeamID() { return teamID; }
  void AddGiveDamage(int index, float damage) { giveDamage[index] += damage; }
  std::array<float, 6> GetGiveDamage() { return giveDamage; }


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
    bool IsPushLeftStick(){
        if (leftStick.x * leftStick.x + leftStick.y * leftStick.y > 0)
            return true;

        return false;
    }
    float GetStickAngle() { return nowAngle; }
    void  SetStickAngle(const float angle) {  stickAngle = angle; }

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

    void AddHitC() { hitDamage++; }

private:
    //�J��������
    void CameraControl();

    //�X�^���X�V
    void StanUpdate(float elapsedTime);

    //�N�[���_�E���X�V
    void CoolUpdate(float elapsedTime);

    void SetLSSkillCoolTime(float time) { LScool.time = time; }
    float GetLSSkillCoolTime() { return LScool.time; }

    //�A�j���[�V�����Ɏg�p����p�x�̕⊮
    float InterpolateAngle(float currentAngle, float targetAngle, float deltaTime, float speed);
    float Lerp(float start, float end, float t);


protected:
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS> moveStateMachine;
    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS> attackStateMachine;
    GameObject* cameraObj = nullptr;

  bool useMoveFlag = true;//false�ɂ����moveState���g��Ȃ�
  float jumpPower = 3.0f;

  bool isStan = false;
  float stanTimer = 0;

  int teamID = 0;   //�����̃`�[��
  int netID = 0;//�ǂ̃N���C�A���g�����̃L������S�����邩
  std::array<float, 6> giveDamage = { 0,0,0,0,0,0 };//�G�ɗ^�����_���[�W�ʂ□���ɗ^������

  //�X�L���N�[���_�E��
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

private:

    // �L�����N�^�[�̑�����͏��
    unsigned int userInput = 0x00;
    unsigned int userInputDown = 0x00;
    unsigned int userInputUp = 0x00;
    DirectX::XMFLOAT2 leftStick = {};
    DirectX::XMFLOAT2 rightStick = {};

    bool dashFlag = false;
    float dashGauge = 10;
    float dashGaugeMax = 10;
    float dashGaugeMinus = 5;
    float dashGaugePlus = 8;
    float dashSpeed = 10;
    float dashRecast = 1;

    AbnormalCondition abnormalcondition;

    //�l�b�g�ɑ���p�̃J�����̌���
    DirectX::XMFLOAT3 fpsCameraDir;

    int charaID;    //�L�����N�^�[���ʗp

    //�쑺�ǉ�  
    float stickAngle = 0.0f;
    float nowAngle = 0.0f;
    float lerpSpeed = 4.0f;

    int hitDamage = 0;
};
