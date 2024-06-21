#pragma once
#include "../System\Component.h"
#include "../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"
#include "Input\Input.h"

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

  // ���O�擾
  const char* GetName() const override { return "Character"; }

  // �J�n����
  void Start() override {};

  // �X�V����
  void Update(float elapsedTime) override;

  // GUI�`��
  void OnGUI() override;

  virtual void MainAttack() {};
  virtual void SubAttack() {};

  virtual void MainSkill() {};
  virtual void SubSkill() {};
  virtual void UltSkill() {};

  virtual void SpaceSkill() {}

  StateMachine<CharacterCom, CHARACTER_ACTIONS>& GetStateMachine() { return stateMachine; }
  float GetJumpPower() { return jumpPower; }

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
  StateMachine<CharacterCom, CHARACTER_ACTIONS> stateMachine;

  float jumpPower = 5.0f;

private:
  // �L�����N�^�[�̑�����͏��
  unsigned int userInput = 0x00;
  unsigned int userInputDown = 0x00;
  unsigned int userInputUp = 0x00;
  DirectX::XMFLOAT2 leftStick = {};
  DirectX::XMFLOAT2 rightStick = {};

};
