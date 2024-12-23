#define _WINSOCKAPI_  // windows.hを定義した際に、winsock.hを自動的にインクルードしない
#include <windows.h>
#include <math.h>
#include <Xinput.h>
#include "Input/GamePad.h"

// 更新
void GamePad::Update()
{
  axisL_ = {};
  axisR_ = {};
  triggerL_ = triggerR_ = 0.0f;

  GamePadButton newButtonState = 0;

  // ボタン情報取得
  XINPUT_STATE xinputState;
  if (XInputGetState(slot_, &xinputState) == ERROR_SUCCESS)
  {
    //XINPUT_CAPABILITIES caps;
    //XInputGetCapabilities(m_slot, XINPUT_FLAG_GAMEPAD, &caps);
    XINPUT_GAMEPAD& pad = xinputState.Gamepad;

    if (pad.wButtons & XINPUT_GAMEPAD_DPAD_UP)					newButtonState |= BTN_UP;
    if (pad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)				newButtonState |= BTN_RIGHT;
    if (pad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)				newButtonState |= BTN_DOWN;
    if (pad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)				newButtonState |= BTN_LEFT;
    if (pad.wButtons & XINPUT_GAMEPAD_A)						newButtonState |= BTN_A;
    if (pad.wButtons & XINPUT_GAMEPAD_B)						newButtonState |= BTN_B;
    if (pad.wButtons & XINPUT_GAMEPAD_X)						newButtonState |= BTN_X;
    if (pad.wButtons & XINPUT_GAMEPAD_Y)						newButtonState |= BTN_Y;
    if (pad.wButtons & XINPUT_GAMEPAD_START)					newButtonState |= BTN_START;
    if (pad.wButtons & XINPUT_GAMEPAD_BACK)						newButtonState |= BTN_BACK;
    if (pad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)				newButtonState |= BTN_LEFT_THUMB;
    if (pad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)				newButtonState |= BTN_RIGHT_THUMB;
    if (pad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)			newButtonState |= BTN_LEFT_SHOULDER;
    if (pad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)			newButtonState |= BTN_RIGHT_SHOULDER;
    if (pad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)	newButtonState |= BTN_LEFT_TRIGGER;
    if (pad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)	newButtonState |= BTN_RIGHT_TRIGGER;

    if ((pad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && pad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
      (pad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && pad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
    {
      pad.sThumbLX = 0;
      pad.sThumbLY = 0;
    }

    if ((pad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
      (pad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
    {
      pad.sThumbRX = 0;
      pad.sThumbRY = 0;
    }

    triggerL_ = static_cast<float>(pad.bLeftTrigger) / 255.0f;
    triggerR_ = static_cast<float>(pad.bRightTrigger) / 255.0f;
    axisL_.x = static_cast<float>(pad.sThumbLX) / static_cast<float>(0x8000);
    axisL_.y = static_cast<float>(pad.sThumbLY) / static_cast<float>(0x8000);
    axisR_.x = static_cast<float>(pad.sThumbRX) / static_cast<float>(0x8000);
    axisR_.y = static_cast<float>(pad.sThumbRY) / static_cast<float>(0x8000);
  }
  else
  {
#if 0
    // XInputで入力情報が取得出来なかった場合はWindowsAPIで取得する
    JOYINFOEX joyInfo;
    joyInfo.dwSize = sizeof(JOYINFOEX);
    joyInfo.dwFlags = JOY_RETURNALL;	// 全ての情報を取得

    if (joyGetPosEx(JOYSTICKID2, &joyInfo) == JOYERR_NOERROR)
    {
      // 製品IDをチェックしてPS4コントローラーだけ対応する
      static const WORD PS4_PID = 1476;

      JOYCAPS joy_caps;
      if (joyGetDevCaps(JOYSTICKID2, &joy_caps, sizeof(JOYCAPS)) == JOYERR_NOERROR)
      {
        // 十字キー
        if (joyInfo.dwPOV != 0xFFFF)
        {
          static const int povBit[8] =
          {
            BTN_UP,					// 上
            BTN_RIGHT | BTN_UP,		// 右上
            BTN_RIGHT,				// 右
            BTN_RIGHT | BTN_DOWN,	// 右下
            BTN_DOWN,				// 下
            BTN_LEFT | BTN_DOWN,	// 左下
            BTN_LEFT,				// 左
            BTN_LEFT | BTN_UP		// 左上
          };
          int angle = joyInfo.dwPOV / 4500;
          newButtonState |= povBit[angle];
        }
        //if (joy_caps.wPid == PS4_PID)
        {
          // ボタン情報
          if (joyInfo.dwButtons & JOY_BUTTON1)  newButtonState |= BTN_Y;				//□
          if (joyInfo.dwButtons & JOY_BUTTON2)  newButtonState |= BTN_B;				//×
          if (joyInfo.dwButtons & JOY_BUTTON3)  newButtonState |= BTN_A;				//〇
          if (joyInfo.dwButtons & JOY_BUTTON4)  newButtonState |= BTN_X;				//△
          if (joyInfo.dwButtons & JOY_BUTTON5)  newButtonState |= BTN_LEFT_SHOULDER;	//L1
          if (joyInfo.dwButtons & JOY_BUTTON6)  newButtonState |= BTN_RIGHT_SHOULDER;	//R1
          if (joyInfo.dwButtons & JOY_BUTTON7)  newButtonState |= BTN_LEFT_TRIGGER;	//L2
          if (joyInfo.dwButtons & JOY_BUTTON8)  newButtonState |= BTN_RIGHT_TRIGGER;	//R2
          if (joyInfo.dwButtons & JOY_BUTTON9)  newButtonState |= BTN_BACK;			//SHARE
          if (joyInfo.dwButtons & JOY_BUTTON10) newButtonState |= BTN_START;			//OPTIONS
          if (joyInfo.dwButtons & JOY_BUTTON11) newButtonState |= BTN_LEFT_THUMB;		//L3
          if (joyInfo.dwButtons & JOY_BUTTON12) newButtonState |= BTN_RIGHT_THUMB;	//R3
          //if (joyInfo.dwButtons & JOY_BUTTON13) newButtonState |= BTN_?;	// PS
          //if (joyInfo.dwButtons & JOY_BUTTON14) newButtonState |= BTN_?;	// Touch

          // 左スティック
          axisLx_ = static_cast<int>(joyInfo.dwXpos - 0x7FFF) / static_cast<float>(0x8000);
          axisLy_ = -static_cast<int>(joyInfo.dwYpos - 0x7FFF) / static_cast<float>(0x8000);

          // 右スティック
          axisRx_ = static_cast<int>(joyInfo.dwZpos - 0x7FFF) / static_cast<float>(0x8000);
          axisRy_ = -static_cast<int>(joyInfo.dwRpos - 0x7FFF) / static_cast<float>(0x8000);

          // LRトリガー
          triggerL_ = static_cast<float>(joyInfo.dwVpos) / static_cast<float>(0xFFFF);
          triggerR_ = static_cast<float>(joyInfo.dwUpos) / static_cast<float>(0xFFFF);

          if (axisLx_ > -0.25f && axisLx_ < 0.25f) axisLx_ = 0.0f;
          if (axisRx_ > -0.25f && axisRx_ < 0.25f) axisRx_ = 0.0f;
        }
      }
    }
#endif
  }

  // キーボードでエミュレーション
  {
    float lx = 0.0f;
    float ly = 0.0f;
    float rx = 0.0f;
    float ry = 0.0f;
    if (GetAsyncKeyState('W') & 0x8000) ly = 1.0f;
    if (GetAsyncKeyState('A') & 0x8000) lx = -1.0f;
    if (GetAsyncKeyState('S') & 0x8000) ly = -1.0f;
    if (GetAsyncKeyState('D') & 0x8000) lx = 1.0f;
    if (GetAsyncKeyState('I') & 0x8000) ry = 1.0f;
    if (GetAsyncKeyState('J') & 0x8000) rx = -1.0f;
    if (GetAsyncKeyState('K') & 0x8000) ry = -1.0f;
    if (GetAsyncKeyState('L') & 0x8000) rx = 1.0f;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) newButtonState |= BTN_A;
    if (GetAsyncKeyState('Q') & 0x8000) newButtonState |= BTN_B;
    if (GetAsyncKeyState('E') & 0x8000) newButtonState |= BTN_X;
    if (GetAsyncKeyState('R') & 0x8000) newButtonState |= BTN_Y;
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000)newButtonState |= BTN_LEFT_SHOULDER;
    if (GetAsyncKeyState('C') & 0x8000)newButtonState |= BTN_RIGHT_SHOULDER;
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)newButtonState |= BTN_LEFT_TRIGGER;
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)newButtonState |= BTN_RIGHT_TRIGGER;
    if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)newButtonState |= BTN_LCONTROL;

    //if (GetAsyncKeyState(VK_UP) & 0x8000)		newButtonState |= BTN_UP;
    //if (GetAsyncKeyState(VK_RIGHT) & 0x8000)	newButtonState |= BTN_RIGHT;
    //if (GetAsyncKeyState(VK_DOWN) & 0x8000)		newButtonState |= BTN_DOWN;
    //if (GetAsyncKeyState(VK_LEFT) & 0x8000)		newButtonState |= BTN_LEFT;

#if 1
    if (newButtonState & BTN_UP)    ly = 1.0f;
    if (newButtonState & BTN_RIGHT) lx = 1.0f;
    if (newButtonState & BTN_DOWN)  ly = -1.0f;
    if (newButtonState & BTN_LEFT)  lx = -1.0f;
#endif

    if (lx >= 1.0f || lx <= -1.0f || ly >= 1.0f || ly <= -1.0)
    {
      float power = ::sqrtf(lx * lx + ly * ly);
      axisL_.x = lx / power;
      axisL_.y = ly / power;
    }

    if (rx >= 1.0f || rx <= -1.0f || ry >= 1.0f || ry <= -1.0)
    {
      float power = ::sqrtf(rx * rx + ry * ry);
      axisR_.x = rx / power;
      axisR_.y = ry / power;
    }
  }



  // ボタン情報の更新
  {
    buttonState_[1] = buttonState_[0];	// スイッチ履歴
    buttonState_[0] = newButtonState;

    buttonDown_ = ~buttonState_[1] & newButtonState;	// 押した瞬間
    buttonUp_ = ~newButtonState & buttonState_[1];	// 離した瞬間
  }
}
