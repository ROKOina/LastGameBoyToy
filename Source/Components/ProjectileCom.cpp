#include "ProjectileCom.h"
#include "GameSource/Math/Mathf.h"
#include "Graphics/Graphics.h"
#include "Components/TransformCom.h"
#include "Components/System/RayCastManager.h"

#define GRAVITY 9.8f

void ProjectileCom::Update(float elapsedTime)
{
  HorizonUpdate(elapsedTime);
  UpdateGravity(elapsedTime);

  CheckHitGround();
  UpdateFriction(elapsedTime);

  ApplyVelocity(elapsedTime);
}

void ProjectileCom::Rebound(const DirectX::XMFLOAT3& normal)
{
  float vn = Mathf::Dot(normal, context.velocity);
  if (vn > 0)return;
  vn *= -(context.restitution + 1);

  context.velocity = normal * vn + context.velocity;
}

void ProjectileCom::CheckHitGround()
{
  DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();
  float vectorY = context.velocity.y + gravitySimulate;

  // �n�ʂƂ̒��n����
  {
    // ���C�L���X�g�p�̃I�t�Z�b�g
    TransformCom* transform = GetGameObject()->transform_.get();

    // ���C�̊J�n�ʒu�͑�����菭����
    DirectX::XMFLOAT3 start = transform->GetWorldPosition();
    start.y += STEP_OFFSET;

    // ���C�̏I�_�ʒu�͈ړ���̈ʒu
    DirectX::XMFLOAT3 end = transform->GetWorldPosition();
    end.y += vectorY;

    // ����
    onGround = RayCastManager::Instance().RayCast(start, end);
  }
}

void ProjectileCom::UpdateFriction(float elapsedTime)
{
  // ���C��
  float friction;
  {
    // ���E�̃X�s�[�h
    float worldSpeed = Graphics::Instance().GetWorldSpeed();
    // �I�u�W�F�N�g�̃X�s�[�h
    float objSpeed = GetGameObject()->GetObjSpeed();

    friction = (onGround) ? context.frictionGround : context.frictionAir;
    friction *= (elapsedTime * worldSpeed * objSpeed);
  }

  DirectX::XMFLOAT3 horizonVelocity = { context.velocity.x, 0, context.velocity.z };

  // ���C��
  if (Mathf::Dot(horizonVelocity, horizonVelocity) > 0.0f)
  {
    DirectX::XMFLOAT3 friVelocity = horizonVelocity * -friction;
    context.velocity += friVelocity;
  }
  else
  {
    context.velocity.x = 0;
    context.velocity.z = 0;
  }
}

void ProjectileCom::UpdateGravity(float elapsedTime)
{
  // ���E�̃X�s�[�h
  float worldSpeed = Graphics::Instance().GetWorldSpeed();
  // �I�u�W�F�N�g�̃X�s�[�h
  float objSpeed = GetGameObject()->GetObjSpeed();
  float gravity = GRAVITY * (elapsedTime * worldSpeed * objSpeed);

  gravitySimulate += gravity;
  gravitySimulate = min(gravitySimulate, MAX_GRAVITY);
}

void ProjectileCom::HorizonUpdate(float elapsedTime)
{
  DirectX::XMFLOAT3 horizonVelocity = { context.velocity.x, 0, context.velocity.z };
  float horiLengthSq = Mathf::Dot(horizonVelocity, horizonVelocity);

  // �ő呬�x�𒴂��Ă���ꍇ�͐�������
  if (horiLengthSq > context.maxSpeed * context.maxSpeed)
  {
    DirectX::XMFLOAT3 newMaxVelocity = Mathf::Normalize(horizonVelocity) * context.maxSpeed;
    context.velocity.x = newMaxVelocity.x;
    context.velocity.z = newMaxVelocity.z;
  }
}

void ProjectileCom::ApplyVelocity(float elapsedTime)
{
  DirectX::XMFLOAT3 oldPosition = GetGameObject()->transform_->GetWorldPosition();
  DirectX::XMFLOAT3 newPosition = oldPosition + context.velocity + DirectX::XMFLOAT3(0, gravitySimulate, 0);

  if (context.isCollideTerrain) {
    RayCastManager::Result hit;
    if (RayCastManager::Instance().RayCast(oldPosition, newPosition, hit)) {
      newPosition = hit.position;

      if (context.isHitOnce == false) 
        Rebound(hit.normal);
    }
  }

  GetGameObject()->transform_->SetWorldPosition(newPosition);
}
