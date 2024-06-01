#pragma once

using GamePadButton = unsigned int;
using GamePadVibration = unsigned int;

// �Q�[���p�b�h
class GamePad
{
public:
    static const GamePadButton BTN_UP = (1 << 0ull);
    static const GamePadButton BTN_RIGHT = (1 << 1ull);
    static const GamePadButton BTN_DOWN = (1 << 2ull);
    static const GamePadButton BTN_LEFT = (1 << 3ull);
    static const GamePadButton BTN_A = (1 << 4ull);
    static const GamePadButton BTN_B = (1 << 5ull);
    static const GamePadButton BTN_X = (1 << 6ull);
    static const GamePadButton BTN_Y = (1 << 7ull);
    static const GamePadButton BTN_START = (1 << 8ull);
    static const GamePadButton BTN_BACK = (1 << 9ull);
    static const GamePadButton BTN_LEFT_THUMB = (1 << 10ull);
    static const GamePadButton BTN_RIGHT_THUMB = (1 << 11ull);
    static const GamePadButton BTN_LEFT_SHOULDER = (1 << 12ull);
    static const GamePadButton BTN_RIGHT_SHOULDER = (1 << 13ull);
    static const GamePadButton BTN_LEFT_TRIGGER = (1 << 14ull);
    static const GamePadButton BTN_RIGHT_TRIGGER = (1 << 15ull);
    static const GamePadButton BTN_W = (1 << 16ull);

public:
    GamePad() {}
    ~GamePad() {}

    // �X�V
    void Update();

    // �X���b�g�ݒ�
    void SetSlot(int slot) { this->slot = slot; }

    // �{�^�����͏�Ԃ̎擾
    GamePadButton GetButton() const { return buttonState[0]; }

    // �{�^��������Ԃ̎擾
    GamePadButton GetButtonDown() const { return buttonDown; }

    // �{�^�������Ԃ̎擾
    GamePadButton GetButtonUp() const { return buttonUp; }

    // ���X�e�B�b�NX�����͏�Ԃ̎擾
    float GetAxisLX() const { return axisLx; }

    // ���X�e�B�b�NY�����͏�Ԃ̎擾
    float GetAxisLY() const { return axisLy; }

    // �E�X�e�B�b�NX�����͏�Ԃ̎擾
    float GetAxisRX() const { return axisRx; }

    // �E�X�e�B�b�NY�����͏�Ԃ̎擾
    float GetAxisRY() const { return axisRy; }

    // ���g���K�[���͏�Ԃ̎擾
    float GetTriggerL() const { return triggerL; }

    // �E�g���K�[���͏�Ԃ̎擾
    float GetTriggerR() const { return triggerR; }

    //�o�C�u�E,��
    GamePadVibration GetVleft()const { return Vleft; }
    GamePadVibration GetVright()const { return Vright; }

private:
    GamePadButton		buttonState[2] = { 0 };
    GamePadButton		buttonDown = 0;
    GamePadButton		buttonUp = 0;
    GamePadVibration    Vleft = 0;
    GamePadVibration    Vright = 0;
    float				axisLx = 0.0f;
    float				axisLy = 0.0f;
    float				axisRx = 0.0f;
    float				axisRy = 0.0f;
    float				triggerL = 0.0f;
    float				triggerR = 0.0f;
    float               vibration = 0.0f;
    int					slot = 0;
};
