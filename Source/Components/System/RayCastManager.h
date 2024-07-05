#pragma once

#include <vector>
#include <memory>
#include <DirectXMath.h>

class GameObject;
class RayCollisionCom;

// レイキャストマネージャー( シングルトン )
class RayCastManager
{
private:
  RayCastManager() { }
  ~RayCastManager() {}

public:
  // レイキャストの結果情報
  struct Result
  {
    DirectX::XMFLOAT3 position = { 0,0,0 }; // レイとポリゴンの交点
    DirectX::XMFLOAT3 normal = { 0,0,0 };   // 衝突したポリゴンの法線ベクトル
    float distance = 0.0f;                  // レイの始点から交点までの距離
    int materialIndex = -1;                 // 衝突したポリゴンのマテリアル番号
    float slopeRate = 0.0f;                 // 交点の傾斜率
    std::weak_ptr<GameObject> hitObject;    // 当たったオブジェクト
  };

public:
  // インスタンス取得
  static RayCastManager& Instance()
  {
    static RayCastManager instance;
    return instance;
  }

  // 登録
  void Register(std::shared_ptr<RayCollisionCom> newCollision) { collisionModels.emplace_back(newCollision); }

  // 除外
  void Remove(std::shared_ptr<RayCollisionCom> removeCollision);

  // 判定 ( 当たった判定のみ取得 )
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end) { return RayCast(start, end, nullptr, nullptr, nullptr); };

  // 判定 ( 当たった位置のみ取得 )
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, DirectX::XMFLOAT3& position) { return RayCast(start, end, &position, nullptr, nullptr); };

  // 判定 ( 当たったオブジェクトのみ取得 )
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, std::weak_ptr<GameObject> hitObject) { return RayCast(start, end, nullptr, nullptr, &hitObject); };

  // 判定 ( 当たった情報を取得 )
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, Result& result) { return RayCast(start, end, &result.position, &result, &result.hitObject); };

private:
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, DirectX::XMFLOAT3* positionPtr, Result* resultPtr, std::weak_ptr<GameObject>* objectPtr);

  bool Collide(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, std::weak_ptr<RayCollisionCom> collision, Result* hitResult);

  bool RayVsBox(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& positive, const DirectX::XMFLOAT3& negative);

private:
  std::vector<std::weak_ptr<RayCollisionCom>> collisionModels;

};
