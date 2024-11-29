#include "ScarecrowCom.h"
#include "Math/Mathf.h"
#include "Graphics/Graphics.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\System\TransformCom.h"
#include <Component\MoveSystem\MovementCom.h>
#include "Component\Sprite\Sprite.h"

//初期設定  
void ScarecrowCom::Start()
{
    //ステート登録
    state.AddState(ScareCrowState::IDLE, std::make_shared<Scarecrow_IdleState>(this));
    state.AddState(ScareCrowState::RANDOMIDLE, std::make_shared<Scarecrow_RandomIdleState>(this));
    state.AddState(ScareCrowState::MOVE, std::make_shared<Scarecrow_MoveState>(this));
    state.AddState(ScareCrowState::DEATH, std::make_shared<Scarecrow_DeathState>(this));

    //初期ステート登録
    state.ChangeState(ScareCrowState::IDLE);
}

//更新処理
void ScarecrowCom::Update(float elapsedTime)
{
    state.Update(elapsedTime);
}

//IMGUI
void ScarecrowCom::OnGUI()
{
    state.ImGui();
}

