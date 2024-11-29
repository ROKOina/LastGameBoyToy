#include "GateGimmickCom.h"
#include "Scene\ScenePVE\PVEDirection.h"
#include "StateMachine\Behaviar\GateGimmickState.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\System\TransformCom.h"

void GateGimmick::Start()
{
    state.AddState(GimmickState::IDLE, std::make_shared<GateGimmick_IdleState>(this));
    state.AddState(GimmickState::UP, std::make_shared<GateGimmick_UpState>(this));
    state.AddState(GimmickState::DOWN, std::make_shared<GateGimmick_DownState>(this));

    state.ChangeState(GimmickState::IDLE);
}

void GateGimmick::Update(float elapsedTime)
{
    RigidBodyCom* rigid = GetGameObject()->GetComponent<RigidBodyCom>().get();
    PxTransform trans = rigid->GetPxTransform();

    DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
    trans.p = { pos.x, pos.y, pos.z };

    //ステート更新処理
    state.Update(elapsedTime);
}
