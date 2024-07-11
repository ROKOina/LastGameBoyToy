#pragma once

#include <DirectXMath.h>
#include "System\Component.h"

// 挙動制御用コンポーネントの生成に使用するコンテキスト
struct ProjectileContext
{
  DirectX::XMFLOAT3 velocity = {};  // 移動するベクトル

  float acceleration    = 0.0f;     // 加速度
  float maxSpeed        = 0.0f;     // 最大速度
  float restitution     = 0.8f;		  // 反発係数
  float frictionGround  = 1;        // 地面との摩擦
  float frictionAir     = 1;        // 空気抵抗

  bool isApplyGravity   = true;     // 重力の影響を受けるか
  bool isCollideTerrain = true;     // 地形との当たり判定をするか
};

// 弾・グレネードなど投射物用の挙動制御コンポーネント
class ProjectileCom :public Component
{
public:
  ProjectileCom(const ProjectileContext& context);

  // 名前取得
  const char* GetName() const override { return "ProjectileCom"; }

  // 更新処理
  void Update(float elapsedTime)override;

  // TRUE = 地形と当たったか
  bool HitTerrain() { return hitTerrain; }
  bool OnGround() { return onGround; }

private:
  // 地形に当たった時にバウンドする
  void Rebound(const DirectX::XMFLOAT3& normal);

  // 地面との当たり判定
  void CheckHitGround(const float& simulateSpeed);

  // 摩擦力の更新
  void UpdateFriction(const float& simulateSpeed);

  // 重力の更新
  void UpdateGravity(const float& simulateSpeed);

  // 横方向移動更新
  void VelocityAcceleration(const float& simulateSpeed);

  // 速力を反映
  void ApplyVelocity(const float& simulateSpeed);

private:
  const float MASS = 1.0f;          // 質量
  const float MAX_GRAVITY = 10.0f;  // 最大重力加速度
  const float STEP_OFFSET = 0.5f;   // レイキャスト用のオフセット値

  ProjectileContext context;
  float gravitySimulate = 0.0f;

  bool isSimulateEnd  = false;    // 移動量が0になったか
  bool onGround       = false;    // 接地しているか
  bool hitTerrain   = false;    // 地形に当たったか
};