#include "ProjectileCom.h"
#include "GameSource/Math/Mathf.h"
#include "Graphics/Graphics.h"
#include "Components/TransformCom.h"
#include "Components/System/RayCastManager.h"

#include "Graphics/DebugRenderer/LineRenderer.h"

#define GRAVITY 0.98f

ProjectileCom::ProjectileCom(const ProjectileContext& context) :context(context)
{
}

void ProjectileCom::Update(float elapsedTime)
{
  if (isSimulateEnd == true)return;

  hitTerrain = false;

  // 世界のスピード
  float worldSpeed = Graphics::Instance().GetWorldSpeed();
  // オブジェクトのスピード
  float objSpeed = GetGameObject()->GetObjSpeed();

  float simulateSpeed = elapsedTime * worldSpeed * objSpeed;

  VelocityAcceleration(simulateSpeed);
  UpdateGravity(simulateSpeed);

  CheckHitGround(simulateSpeed);
  UpdateFriction(simulateSpeed);

  ApplyVelocity(simulateSpeed);
}

void ProjectileCom::Rebound(const DirectX::XMFLOAT3& normal)
{
  float vn = Mathf::Dot(normal, context.velocity);
  if (vn > 0)return;
  vn *= -(context.restitution + 1);

  context.velocity = normal * vn + context.velocity;
}

void ProjectileCom::CheckHitGround(const float& simulateSpeed)
{
  DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();
  float vectorY = context.velocity.y * simulateSpeed - gravitySimulate;

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
    RayCastManager::Result hit;
    if (RayCastManager::Instance().RayCast(start, end, hit)) {
      DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();
      position.y = hit.position.y;
      GetGameObject()->transform_->SetWorldPosition(position);

      hitTerrain = true;
      onGround = true;
      gravitySimulate = 0.0f;
    }
    else {
      onGround = false;
    }
  }
}

void ProjectileCom::UpdateFriction(const float& simulateSpeed)
{
  // 摩擦力
  float friction;
  {
    friction = (onGround) ? context.frictionGround : context.frictionAir;
    friction *= simulateSpeed;
  }

  // 摩擦力
  if (Mathf::Dot(context.velocity, context.velocity) > 0.001f)
  {
    DirectX::XMFLOAT3 friVelocity = context.velocity * -friction;
    context.velocity += friVelocity;
  }
  else
  {
    context.velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    isSimulateEnd = true;
  }
}

void ProjectileCom::UpdateGravity(const float& simulateSpeed)
{
  if (context.isApplyGravity == false)return;

  float gravity = GRAVITY * simulateSpeed;

  gravitySimulate += gravity;
  gravitySimulate = min(gravitySimulate, MAX_GRAVITY);
}

void ProjectileCom::VelocityAcceleration(const float& simulateSpeed)
{
  if (context.acceleration == 0.0f)return;

  float lengthSq = Mathf::Dot(context.velocity, context.velocity);

  // 最大速度を超えている場合は制限する
  if (lengthSq > context.maxSpeed * context.maxSpeed)
  {
    DirectX::XMFLOAT3 newMaxVelocity = Mathf::Normalize(context.velocity) * context.maxSpeed;
    context.velocity = newMaxVelocity;
  }
  else
  {
    context.velocity += context.acceleration * simulateSpeed;
  }
}

void ProjectileCom::ApplyVelocity(const float& simulateSpeed)
{
  DirectX::XMFLOAT3 velocity = context.velocity * simulateSpeed + DirectX::XMFLOAT3(0, -gravitySimulate, 0);
  DirectX::XMFLOAT3 oldPosition = GetGameObject()->transform_->GetWorldPosition();
  DirectX::XMFLOAT3 newPosition = oldPosition + context.velocity * simulateSpeed + DirectX::XMFLOAT3(0, -gravitySimulate, 0);
  DirectX::XMFLOAT3 vec = newPosition - oldPosition;

  if (context.isCollideTerrain) {
#ifdef _DEBUG
    Graphics::Instance().GetLineRenderer()->AddVertex(oldPosition, { 1,1,0,1 });
    Graphics::Instance().GetLineRenderer()->AddVertex(newPosition, { 1,1,0,1 });

#endif // _DEBUG

    RayCastManager::Result hit;
    if (RayCastManager::Instance().RayCast(oldPosition, newPosition, hit)) {
      // 壁の法線
      DirectX::XMFLOAT3 normal = hit.normal;

      // 入射ベクトルを法線ベクトルに射影
      float projectionLength = Mathf::Dot(vec, hit.normal);

      // 補正位置の計算
      DirectX::XMFLOAT3 correction = hit.normal * projectionLength + newPosition;

      DirectX::XMFLOAT3 start = hit.position;
      DirectX::XMFLOAT3 end = correction;

      // 補正後の位置が壁にめり込んでいたら
      RayCastManager::Result hit2;
      if (RayCastManager::Instance().RayCast(start, end, hit2))
      {
        newPosition = hit2.position;
        normal = hit2.normal;
      }
      else
      {
        newPosition = correction;
      }

      hitTerrain = true;
      Rebound(normal);
    }
  }

  GetGameObject()->transform_->SetWorldPosition(newPosition);
}
