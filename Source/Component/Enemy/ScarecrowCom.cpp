#include "ScarecrowCom.h"
#include "Math/Mathf.h"
#include "Graphics/Graphics.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\System\TransformCom.h"
#include <Component\MoveSystem\MovementCom.h>
#include "Component\Sprite\Sprite.h"

//�����ݒ�  
void ScarecrowCom::Start()
{
    //�X�e�[�g�o�^
    state.AddState(ScareCrowState::IDLE, std::make_shared<Scarecrow_IdleState>(this));
    state.AddState(ScareCrowState::RANDOMIDLE, std::make_shared<Scarecrow_RandomIdleState>(this));
    state.AddState(ScareCrowState::MOVE, std::make_shared<Scarecrow_MoveState>(this));
    state.AddState(ScareCrowState::DEATH, std::make_shared<Scarecrow_DeathState>(this));

    //�����X�e�[�g�o�^
    state.ChangeState(ScareCrowState::IDLE);
}

//�X�V����
void ScarecrowCom::Update(float elapsedTime)
{
    state.Update(elapsedTime);
}

//IMGUI
void ScarecrowCom::OnGUI()
{
    state.ImGui();
}

