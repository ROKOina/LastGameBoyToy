#include "FPSCameraCom.h"
#include "Graphics/Graphics.h"
#include "Components/TransformCom.h"
#include "Components/Character/CharacterCom.h"
#include "Components/MovementCom.h"

//コンストラクタ
FPSCameraCom::FPSCameraCom() : CameraCom::CameraCom(45, Graphics::Instance().GetScreenWidth() / Graphics::Instance().GetScreenHeight(), 0.1f, 1000.0f)
{
    focuslapelate = 0.45f;
    eyelaperate = 0.01f;
}

// 開始処理
void FPSCameraCom::Start()
{
}

//更新処理
void FPSCameraCom::Update(float elapsedTime)
{
    //基底クラスのカメラを呼ぶ
    CameraCom::Update(elapsedTime);
}

//imgui
void FPSCameraCom::OnGUI()
{
    //基底クラスのカメラを呼ぶ
    CameraCom::OnGUI();
}