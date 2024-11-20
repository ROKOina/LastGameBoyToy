#include "GateGimmickState.h"
#include "Math\Mathf.h"

GateGimmick_BaseState::GateGimmick_BaseState(GateGimmick* owner) : State(owner)
{
    gateGimmick = owner->GetGameObject()->GetComponent<GateGimmick>();
}

void GateGimmick_UpState::Execute(const float& elapsedTime)
{
    
}
