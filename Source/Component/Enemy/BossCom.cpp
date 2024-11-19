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
    //state.AddState(BossState::JUMPATTACKSTART, std::make_shared<Boss_JumpAttackStart>(this));
    //state.AddState(BossState::JUMPATTACKEND, std::make_shared<Boss_JumpAttackEnd>(this));
    state.AddState(BossState::SHOTSTART, std::make_shared<Boss_ShotStart>(this));
    state.AddState(BossState::SHOTLOOP, std::make_shared<Boss_ShotCharge>(this));
    state.AddState(BossState::SHOTEND, std::make_shared<Boss_Shot>(this));
    state.AddState(BossState::UPSHOTSTART, std::make_shared<Boss_UpShotStart>(this));
    state.AddState(BossState::UPSHOTCHARGE, std::make_shared<Boss_UpShotCharge>(this));
    state.AddState(BossState::UPSHOTLOOP, std::make_shared<Boss_UpShotLoop>(this));
    state.AddState(BossState::UPSHOTEND, std::make_shared<Boss_UpShotEnd>(this));

    //���G����
    GetGameObject()->GetComponent<CharaStatusCom>()->SetInvincibleTime(0.1f);

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

    //���ɂ���ꍇ���񂷂�
    BackTurn();

    //DebugPrimitive
    Graphics::Instance().GetDebugRenderer()->DrawCylinder(GetGameObject()->transform_->GetWorldPosition(), GetGameObject()->transform_->GetWorldPosition(), meleerange, 0.1f, { 1.0f,0.0f,0.0f,1.0f });
    Graphics::Instance().GetDebugRenderer()->DrawCylinder(GetGameObject()->transform_->GetWorldPosition(), GetGameObject()->transform_->GetWorldPosition(), longrange, 0.1f, { 0.0f,1.0f,0.0f,1.0f });
    Graphics::Instance().GetDebugRenderer()->DrawCylinder(GetGameObject()->transform_->GetWorldPosition(), GetGameObject()->transform_->GetWorldPosition(), walkrange, 0.1f, { 0.0f,0.0f,1.0f,1.0f });
}

//imgui
void BossCom::OnGUI()
{
    ImGui::DragFloat("meleerange", &meleerange, 1.0f, 0.0f, 50.0f);
    ImGui::DragFloat("longrange", &longrange, 1.0f, 0.0f, 50.0f);
    ImGui::DragFloat("walkrange", &walkrange, 1.0f, 0.0f, 50.0f);

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

//���ɂ���ꍇ���񂷂�
void BossCom::BackTurn()
{
    // �v���C���[�̈ʒu�擾
    DirectX::XMFLOAT3 playerpos = GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 bossPos = GetGameObject()->transform_->GetWorldPosition();

    // �v���C���[�ƃ{�X�Ԃ̋����x�N�g��
    float vx = playerpos.x - bossPos.x;
    float vy = playerpos.y - bossPos.y;
    float vz = playerpos.z - bossPos.z;

    // XZ���ʂł̋������v�Z
    float distXZ = sqrtf(vx * vx + vz * vz);

    // �v���C���[�����̒P�ʃx�N�g��
    vx /= distXZ;
    vz /= distXZ;

    // �{�X�̑O�����x�N�g�� (Y����]�p����Z�o)
    float rotationY = DirectX::XMConvertToRadians(GetGameObject()->transform_->GetEulerRotation().y);
    float frontX = sinf(rotationY);
    float frontZ = cosf(rotationY);

    // ���ςőO�㔻��
    float dot = (frontX * vx) + (frontZ * vz);

    if (dot > 0.0f)
    {
        // �{�X����]
        MoveToTarget(0.0f, 0.001f);  // �����ŁA��]���x��K�؂ɐݒ�
    }
    if (dot < 0.0f)
    {
        // �{�X����]
        MoveToTarget(0.0f, 0.03f);  // �����ŁA��]���x��K�؂ɐݒ�
    }
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