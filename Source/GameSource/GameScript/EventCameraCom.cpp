#include "EventCameraCom.h"
#include "Graphics/Graphics.h"
#include "Components/TransformCom.h"
#include "Components/Character/CharacterCom.h"
#include "Components/MovementCom.h"

//コンストラクタ
EventCameraCom::EventCameraCom() : CameraCom::CameraCom(45, Graphics::Instance().GetScreenWidth() / Graphics::Instance().GetScreenHeight(), 0.1f, 1000.0f)
{
    //focuslapelate = 0.93f;
    //eyelaperate = 0.01f;
}

// 開始処理
void EventCameraCom::Start()
{
}

//更新処理
void EventCameraCom::Update(float elapsedTime)
{
    //基底クラスのカメラを呼ぶ
    CameraCom::Update(elapsedTime);
}

//imgui
void EventCameraCom::OnGUI()
{
    //基底クラスのカメラを呼ぶ
    CameraCom::OnGUI();
}