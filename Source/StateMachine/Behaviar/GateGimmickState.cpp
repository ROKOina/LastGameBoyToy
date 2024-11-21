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
    //�Q�[�g�̏㏸����

    //���݂̈ʒu����㏸�ڕW�ʒu�܂ł̋����擾
    DirectX::XMFLOAT3 dir = gateGimmick.lock()->GetUpPos() - transform.lock()->GetWorldPosition();
    float length = Mathf::Length(dir);

    //�����ɉ����Ĉړ�
    if (length > gateGimmick.lock()->GetMoveSpeed())
    {
        DirectX::XMFLOAT3 movePos = Mathf::Normalize(gateGimmick.lock()->GetUpPos() - transform.lock()->GetWorldPosition()) * gateGimmick.lock()->GetMoveSpeed();
        transform.lock()->SetWorldPosition(movePos + transform.lock()->GetWorldPosition());
    }
    else
    {
        //�ڕW�ʒu�ɒ�������X�e�[�g�ύX
        transform.lock()->SetWorldPosition(gateGimmick.lock()->GetUpPos());
        owner->GetStateMachine().ChangeState(GateGimmick::GimmickState::IDLE);
    }
}

void GateGimmick_DownState::Execute(const float& elapsedTime)
{
    //�Q�[�g�̉��~����

    //���݂̈ʒu���牺�~�ڕW�ʒu�܂ł̋����擾
    DirectX::XMFLOAT3 dir = gateGimmick.lock()->GetDownPos() - transform.lock()->GetWorldPosition();
    float length = Mathf::Length(dir);

    //�����ɉ����Ĉړ�
    if (length > gateGimmick.lock()->GetMoveSpeed())
    {
        DirectX::XMFLOAT3 movePos = Mathf::Normalize(gateGimmick.lock()->GetDownPos() - transform.lock()->GetWorldPosition()) * gateGimmick.lock()->GetMoveSpeed();
        transform.lock()->SetWorldPosition(movePos + transform.lock()->GetWorldPosition());
    }
    else
    {
        //�ڕW�ʒu�ɒ�������X�e�[�g�ύX
        transform.lock()->SetWorldPosition(gateGimmick.lock()->GetDownPos());
        owner->GetStateMachine().ChangeState(GateGimmick::GimmickState::IDLE);
    }
}
