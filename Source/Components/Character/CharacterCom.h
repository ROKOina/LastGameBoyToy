#pragma once
#include "../System\Component.h"
#include "../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"
#include "Input\Input.h"
#include "../AnimationCom.h"
#include <array>

//�v���C���[�p�L�[���͕⏕�N���X
class CharacterInput
{
public:
    static constexpr GamePadButton JumpButton_SPACE = GamePad::BTN_A;
    static constexpr GamePadButton MainSkillButton_Q = GamePad::BTN_RIGHT_SHOULDER;
    static constexpr GamePadButton SubSkillButton_E = GamePad::BTN_LEFT_SHOULDER;
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

    //�����A�j���[�V����
    void DirectionAnimation(
        std::weak_ptr<AnimationCom>animationCom,
        const DirectX::XMFLOAT3& movevec,
        const std::string& forward,
        const std::string& back,
        const std::string& right,
        const std::string& left,
        bool loop,
        const float& blendrate);

    virtual void MainAttack() {};
    virtual void SubAttack() {};

    virtual void MainSkill() {};
    virtual void SubSkill() {};
    virtual void UltSkill() {};

    virtual void SpaceSkill() {}

    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS>& GetAttackStateMachine() { return attackStateMachine; }
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS>& GetMoveStateMachine() { return moveStateMachine; }
    GameObject* GetCameraObj() { return cameraObj; }
    void SetCameraObj(GameObject* obj) { cameraObj = obj; }
    float GetJumpPower() { return jumpPower; }

  void SetHitPoint(float value) { hitPoint = value; }
  float GetHitPoint() { return hitPoint; }
  void AddHitPoint(float value) { hitPoint += value; }
  
  void SetCharaID(int id) { charaID = id; }
  int GetCharaID() { return charaID; }
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

private:
    //�J��������
    void CameraControl();

protected:
    StateMachine<CharacterCom, CHARACTER_MOVE_ACTIONS> moveStateMachine;
    StateMachine<CharacterCom, CHARACTER_ATTACK_ACTIONS> attackStateMachine;
    GameObject* cameraObj = nullptr;

  bool useMoveFlag = true;//false�ɂ����moveState���g��Ȃ�
  float jumpPower = 10.0f;
  float hitPoint = 100.0f;

  int charaID = 0;//�ǂ̃N���C�A���g�����̃L������S�����邩
  std::array<float, 6> giveDamage = { 1,1,1,1,1,1 };//�G�ɗ^�����_���[�W�ʂ□���ɗ^������

private:
    // �L�����N�^�[�̑�����͏��
    unsigned int userInput = 0x00;
    unsigned int userInputDown = 0x00;
    unsigned int userInputUp = 0x00;
    DirectX::XMFLOAT2 leftStick = {};
    DirectX::XMFLOAT2 rightStick = {};
};
