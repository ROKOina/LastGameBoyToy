#include "GateGimmickState.h"
#include "Math\Mathf.h"
#include "Component\System\TransformCom.h"

GateGimmick_BaseState::GateGimmick_BaseState(GateGimmick* owner) : State(owner)
{
    transform = owner->GetGameObject()->transform_;
    gateGimmick = owner->GetGameObject()->GetComponent<GateGimmick>();
}

void GateGimmick_UpState::Execute(const float& elapsedTime)
{
    //ゲートの上昇処理

    //現在の位置から上昇目標位置までの距離取得
    DirectX::XMFLOAT3 dir = gateGimmick.lock()->GetUpPos() - transform.lock()->GetWorldPosition();
    float length = Mathf::Length(dir);

    //距離に応じて移動
    if (length > gateGimmick.lock()->GetMoveSpeed())
    {
        DirectX::XMFLOAT3 movePos = Mathf::Normalize(gateGimmick.lock()->GetUpPos() - transform.lock()->GetWorldPosition()) * gateGimmick.lock()->GetMoveSpeed();
        transform.lock()->SetWorldPosition(movePos + transform.lock()->GetWorldPosition());
    }
    else
    {
        //目標位置に着いたらステート変更
        transform.lock()->SetWorldPosition(gateGimmick.lock()->GetUpPos());
        owner->GetStateMachine().ChangeState(GateGimmick::GimmickState::IDLE);
    }
}

void GateGimmick_DownState::Execute(const float& elapsedTime)
{
    //ゲートの下降処理

    //現在の位置から下降目標位置までの距離取得
    DirectX::XMFLOAT3 dir = gateGimmick.lock()->GetDownPos() - transform.lock()->GetWorldPosition();
    float length = Mathf::Length(dir);

    //距離に応じて移動
    if (length > gateGimmick.lock()->GetMoveSpeed())
    {
        DirectX::XMFLOAT3 movePos = Mathf::Normalize(gateGimmick.lock()->GetDownPos() - transform.lock()->GetWorldPosition()) * gateGimmick.lock()->GetMoveSpeed();
        transform.lock()->SetWorldPosition(movePos + transform.lock()->GetWorldPosition());
    }
    else
    {
        //目標位置に着いたらステート変更
        transform.lock()->SetWorldPosition(gateGimmick.lock()->GetDownPos());
        owner->GetStateMachine().ChangeState(GateGimmick::GimmickState::IDLE);
    }
}
