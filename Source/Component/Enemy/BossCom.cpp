#include "BossCom.h"
#include "Math/Mathf.h"
#include "Graphics/Graphics.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\System\TransformCom.h"
#include <Component\MoveSystem\MovementCom.h>

//�����ݒ�
void BossCom::Start()
{
    //�X�e�[�g�o�^
    state.AddState(BossState::IDLE, std::make_shared<Boss_IdleState>(this));
    state.AddState(BossState::IDLESTOP, std::make_shared<Boss_IdleStopState>(this));
    state.AddState(BossState::MOVE, std::make_shared<Boss_MoveState>(this));
    state.AddState(BossState::SHORTATTACK1, std::make_shared<Boss_SA1>(this));
    state.AddState(BossState::SHORTATTACK2, std::make_shared<Boss_SA2>(this));
    state.AddState(BossState::LARIATSTART, std::make_shared<Boss_LARIATSTART>(this));
    state.AddState(BossState::LARIATLOOP, std::make_shared<Boss_LARIATLOOP>(this));
    state.AddState(BossState::LARIATEND, std::make_shared<Boss_LARIATEND>(this));

    //�����X�e�[�g�o�^
    state.ChangeState(BossState::IDLE);
}

//�X�V����
void BossCom::Update(float elapsedTime)
{
    //�X�e�[�g�X�V����
    state.Update(elapsedTime);

    //�^�[�Q�b�g�ʒu�͏�Ƀv���C���[
    targetposition = GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition();
}

//imgui
void BossCom::OnGUI()
{
    state.ImGui();
}

//���G�֐�
bool BossCom::Search(float range)
{
    DirectX::XMFLOAT3 playerpos = GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition();

    // �v���C���[�Ƃ̍��፷���l������3D�ŋ������������
    float vx = playerpos.x - GetGameObject()->transform_->GetWorldPosition().x;
    float vy = playerpos.y - GetGameObject()->transform_->GetWorldPosition().y;
    float vz = playerpos.z - GetGameObject()->transform_->GetWorldPosition().z;
    float dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist < range)
    {
        // XZ���ʂł̋������v�Z
        float distXZ = sqrtf(vx * vx + vz * vz);

        // �P�ʃx�N�g����
        vx /= distXZ;
        vz /= distXZ;

        // �����x�N�g����
        float frontX = sinf(GetGameObject()->transform_->GetEulerRotation().y);
        float frontZ = cosf(GetGameObject()->transform_->GetEulerRotation().y);

        // 2�̃x�N�g���̓��ϒl�őO�㔻��
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > 0.0f)
        {
            return true;
        }
        if (dot < 0.0f)
        {
            return true;
        }
    }

    return false;
}

// �ڕW�n�_�ֈړ�
void BossCom::MoveToTarget(float movespeed, float turnspeed)
{
    // �^�[�Q�b�g�����ւ̐i�s�x�N�g��
    DirectX::XMFLOAT2 Tvec = TargetVec();

    // �ړ�����
    DirectX::XMFLOAT3 vec = { Tvec.x, 0.0f, Tvec.y };
    GetGameObject()->GetComponent<MovementCom>()->AddForce(vec * movespeed);

    // ��]����
    GetGameObject()->transform_->Turn(vec, turnspeed);
}

//�W�����v
void BossCom::Jump(float power)
{
    // �^�[�Q�b�g�����ւ̐i�s�x�N�g��
    DirectX::XMFLOAT2 vec = TargetVec();

    // �W�����v�͂�ݒ�
    DirectX::XMFLOAT3 jumppower = { vec.x, power, vec.y };
    GetGameObject()->GetComponent<MovementCom>()->AddForce(jumppower);
}

// �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
DirectX::XMFLOAT2 BossCom::TargetVec()
{
    DirectX::XMFLOAT2 vec;
    vec.x = targetposition.x - GetGameObject()->transform_->GetWorldPosition().x;
    vec.y = targetposition.z - GetGameObject()->transform_->GetWorldPosition().z;
    float dist = sqrtf(vec.x * vec.x + vec.y * vec.y);
    vec.x /= dist;
    vec.y /= dist;

    return vec;
}