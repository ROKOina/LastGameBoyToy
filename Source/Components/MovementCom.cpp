#include "MovementCom.h"

#include <imgui.h>
#include "GameSource/Math/Mathf.h"
#include "TransformCom.h"
#include "System/RayCastManager.h"
#include "Graphics\Graphics.h"

// 開始処理
void MovementCom::Start()
{

}

// 更新処理
void MovementCom::Update(float elapsedTime)
{
  VerticalUpdate(elapsedTime);
  HorizonUpdate(elapsedTime);
  VelocityAppPosition(elapsedTime);
};
// GUI描画
void MovementCom::OnGUI()
{
  ImGui::DragFloat3("velocity", &velocity_.x);
  ImGui::DragFloat3("nonMaxSpeedVelocity_", &nonMaxSpeedVelocity_.x);
  ImGui::DragFloat("gravity", &gravity_, 0.1f);
  ImGui::DragFloat("friction", &friction_, 0.1f);
  ImGui::DragFloat("moveMaxSpeed", &moveMaxSpeed_, 0.1f);
  ImGui::DragFloat("moveAcceleration", &moveAcceleration_, 0.1f);
  ImGui::Checkbox("onGround", &onGround_);
}

//縦方向移動更新
void MovementCom::VerticalUpdate(float elapsedTime)
{
  //世界のスピード
  float worldSpeed = Graphics::Instance().GetWorldSpeed();
  //オブジェクトのスピード
  float objSpeed = GetGameObject()->GetObjSpeed();
  float gravity = gravity_ * (elapsedTime * worldSpeed * objSpeed);
  //float gravity = gravity_ * (elapsedTime * Graphics::Instance().GetFPS());
  //float gravity = gravity_ * (elapsedTime*100 /** Graphics::Instance().GetFPS()*/);
  AddForce({ 0,gravity,0 });
  //AddNonMaxSpeedForce({ 0,gravity,0 });
}

//横方向移動更新
void MovementCom::HorizonUpdate(float elapsedTime)
{
  DirectX::XMFLOAT3 horizonVelocity = { velocity_.x,0,velocity_.z };
  DirectX::XMVECTOR HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
  float horiLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(HorizonVelocity));


  //最大速度設定
  if (horiLength > moveMaxSpeed_)
  {
    DirectX::XMVECTOR MaxSpeed = DirectX::XMVectorScale(DirectX::XMVector3Normalize(HorizonVelocity), moveMaxSpeed_);
    DirectX::XMFLOAT3 newMaxVelocity;
    DirectX::XMStoreFloat3(&newMaxVelocity, MaxSpeed);
    velocity_.x = newMaxVelocity.x;
    velocity_.z = newMaxVelocity.z;

    horizonVelocity = { velocity_.x,0,velocity_.z };
    HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    horiLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(HorizonVelocity));
  }

  //世界のスピード
  float worldSpeed = Graphics::Instance().GetWorldSpeed();
  //オブジェクトのスピード
  float objSpeed = GetGameObject()->GetObjSpeed();


  float friction = friction_ * (elapsedTime * worldSpeed * objSpeed);
  //float friction = friction_ * (elapsedTime * Graphics::Instance().GetFPS() * worldSpeed);
  //float friction = friction_ * (elapsedTime*100 /** Graphics::Instance().GetFPS()*/);
  //摩擦力
  if (horiLength > 0.1f)
  {
    DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(DirectX::XMVectorScale(HorizonVelocity, -1), friction);
    //DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(DirectX::XMVector3Normalize(HorizonVelocity), -friction);
    //DirectX::XMFLOAT3 newVelocity;
    //DirectX::XMStoreFloat3(&newVelocity, FriVelocity);
    DirectX::XMStoreFloat3(&velocity_, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&velocity_), FriVelocity));
  }
  else
  {
    velocity_.x = 0;
    velocity_.z = 0;
  }

  //最大速度の無い摩擦計算
  horizonVelocity = { nonMaxSpeedVelocity_.x,0,nonMaxSpeedVelocity_.z };
  HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
  horiLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(HorizonVelocity));

  //摩擦力
  if (horiLength > friction)
  {
    DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(DirectX::XMVectorScale(HorizonVelocity, -1), friction);
    DirectX::XMFLOAT3 newVelocity;
    DirectX::XMStoreFloat3(&newVelocity, FriVelocity);
    AddNonMaxSpeedForce(newVelocity);
  }
  else
  {
    nonMaxSpeedVelocity_.x = 0;
    nonMaxSpeedVelocity_.z = 0;
  }
}

//速力を更新
void MovementCom::VelocityAppPosition(float elapsedTime)
{
  DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();
  DirectX::XMFLOAT3 velocity;
  velocity.x = velocity_.x + nonMaxSpeedVelocity_.x;
  velocity.y = velocity_.y + nonMaxSpeedVelocity_.y;
  velocity.z = velocity_.z + nonMaxSpeedVelocity_.z;

  //世界のスピード
  float worldSpeed = Graphics::Instance().GetWorldSpeed();
  //オブジェクトのスピード
  float objSpeed = GetGameObject()->GetObjSpeed();

  position.x += velocity.x * (elapsedTime * worldSpeed * objSpeed);
  position.z += velocity.z * (elapsedTime * worldSpeed * objSpeed);

  // 垂直方向の移動量
  float verticalVelocity = velocity.y * (elapsedTime * worldSpeed * objSpeed);

  // 地面との着地判定
  {
    // レイキャスト用のオフセット
    float stepOffset = 1.0f;
    TransformCom* transform = GetGameObject()->transform_.get();

    // レイの開始位置は足元より少し上
    DirectX::XMFLOAT3 start = transform->GetWorldPosition();
    start.y += stepOffset;

    // レイの終点位置は移動後の位置
    DirectX::XMFLOAT3 end = transform->GetWorldPosition();
    end.y += verticalVelocity;

    // 判定
    RayCastManager::Result hit;
    if (RayCastManager::Instance().RayCast(start, end, hit)) {
      // 地面に接地している

      // 傾斜率の計算
      DirectX::XMVECTOR Horizon = DirectX::XMLoadFloat3(&hit.normal);
      Horizon = DirectX::XMVectorSetY(Horizon, 0);
      float horizon = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Horizon));

      // 値が１に近いほど壁になる
      float slopeRate = 1 - (hit.normal.y / (hit.normal.y + horizon));

      // 傾斜率が閾値以上の場合、法線方向に押し出す
      if (slopeRate >= slopeThreshold)
        position += hit.normal * elapsedTime;
      else
        position.y = hit.position.y;

      velocity_.y = 0;
      nonMaxSpeedVelocity_.y = 0;
      onGround_ = true;
    }
    else {
      position.y += velocity.y * (elapsedTime * worldSpeed * objSpeed);
      onGround_ = false;
    }

    GetGameObject()->transform_->SetWorldPosition(position);
  }

  GetGameObject()->transform_->SetWorldPosition(position);
}
