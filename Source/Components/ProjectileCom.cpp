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

  // 地面との着地判定
  {
    // レイキャスト用のオフセット
    TransformCom* transform = GetGameObject()->transform_.get();

    // レイの開始位置は足元より少し上
    DirectX::XMFLOAT3 start = transform->GetWorldPosition();
    start.y += STEP_OFFSET;

    // レイの終点位置は移動後の位置
    DirectX::XMFLOAT3 end = transform->GetWorldPosition();
    end.y += vectorY;

    // 判定
    onGround = RayCastManager::Instance().RayCast(start, end);
  }
}

void ProjectileCom::UpdateFriction(float elapsedTime)
{
  // 摩擦力
  float friction;
  {
    // 世界のスピード
    float worldSpeed = Graphics::Instance().GetWorldSpeed();
    // オブジェクトのスピード
    float objSpeed = GetGameObject()->GetObjSpeed();

    friction = (onGround) ? context.frictionGround : context.frictionAir;
    friction *= (elapsedTime * worldSpeed * objSpeed);
  }

  DirectX::XMFLOAT3 horizonVelocity = { context.velocity.x, 0, context.velocity.z };

  // 摩擦力
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
  // 世界のスピード
  float worldSpeed = Graphics::Instance().GetWorldSpeed();
  // オブジェクトのスピード
  float objSpeed = GetGameObject()->GetObjSpeed();
  float gravity = GRAVITY * (elapsedTime * worldSpeed * objSpeed);

  gravitySimulate += gravity;
  gravitySimulate = min(gravitySimulate, MAX_GRAVITY);
}

void ProjectileCom::HorizonUpdate(float elapsedTime)
{
  DirectX::XMFLOAT3 horizonVelocity = { context.velocity.x, 0, context.velocity.z };
  float horiLengthSq = Mathf::Dot(horizonVelocity, horizonVelocity);

  // 最大速度を超えている場合は制限する
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
