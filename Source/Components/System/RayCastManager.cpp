#include "RayCastManager.h"
#include "GameSource/Math/Mathf.h"
#include "Components/RayCollisionCom.h"
#include "Components/TransformCom.h"
#include <queue>
#include <assert.h>

void RayCastManager::Remove(std::shared_ptr<RayCollisionCom> removeCollision)
{
  auto& removeItr = std::remove_if(collisionModels.begin(), collisionModels.end(), 
    [&](std::weak_ptr<RayCollisionCom>& collision) {return collision.lock() == removeCollision; }
  );
  assert(removeItr != collisionModels.end());

  collisionModels.erase(removeItr);
}

bool RayCastManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, DirectX::XMFLOAT3* positionPtr, Result* resultPtr, std::weak_ptr<GameObject>* objectPtr)
{
  bool isHit = false;

  // スタートと交点の距離を保存する
  float length = FLT_MAX;

  // 一番近い交点の情報
  Result nearHit = {};

  for (int i = 0; i < collisionModels.size(); ++i)
  {
    // レイキャスト
    if (Collide(start, end, collisionModels[i], resultPtr)) {
      if (positionPtr == nullptr && resultPtr == nullptr && objectPtr == nullptr)return true;

      isHit = true;

      //  二点間距離を求める
      DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
      DirectX::XMVECTOR Point = DirectX::XMLoadFloat3(&resultPtr->position);
      DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Point, Start);

      float vecLength = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

      // 今までの距離より短ければ代入
      if (vecLength < length)
      {
        // 最短距離の更新
        length = vecLength;

        // 最短距離のヒット情報を更新
        nearHit = *resultPtr;
      }
    }
  }

  // 情報を代入
  if (positionPtr)*positionPtr = nearHit.position;
  if (objectPtr)*objectPtr = nearHit.hitObject;
  if (resultPtr) {
    *resultPtr = nearHit;

    // 傾斜率の計算
    nearHit.normal.y = 0;
    float horizon = sqrtf(Mathf::Dot(nearHit.normal));

    // 値が１に近いほど壁になる
    resultPtr->slopeRate = 1 - (resultPtr->normal.y / (resultPtr->normal.y + horizon));
  }

  return isHit;
}

bool RayCastManager::Collide(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, std::weak_ptr<RayCollisionCom> collision, Result* hitResult)
{
  const RayCollisionCom::Collision& collisionData = collision.lock()->GetCollision();
  const std::vector<RayCollisionCom::Collision::PolygonGroupe>& groups = collisionData.polygonGroupes;
  const std::vector<std::vector<RayCollisionCom::CollisionVertex>>& vertices = collisionData.vertices;

  auto& woldTransform = collision.lock()->GetGameObject()->transform_->GetWorldTransform();
  DirectX::XMMATRIX WorldMat = DirectX::XMLoadFloat4x4(&woldTransform);
  DirectX::XMMATRIX InverseTransform = DirectX::XMMatrixInverse(nullptr, WorldMat);

  // レイを自身を中心とした空間に変換
  DirectX::XMVECTOR LocalStart = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&start), InverseTransform);
  DirectX::XMVECTOR LocalEnd = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&end), InverseTransform);

  // AABBとの当たり判定
  std::queue<int> hitGroups;
  {
    DirectX::XMFLOAT3 localStart;
    DirectX::XMFLOAT3 localEnd;
    DirectX::XMStoreFloat3(&localStart, LocalStart);
    DirectX::XMStoreFloat3(&localEnd, LocalEnd);

    for (int i = 0; i < groups.size(); ++i)
    {
      DirectX::XMFLOAT3 positive = groups[i].aabbCenter + groups[i].aabbRadius;
      DirectX::XMFLOAT3 negative = groups[i].aabbCenter - groups[i].aabbRadius;

      if (RayVsBox(localStart, localEnd, positive, negative)) {
        hitGroups.push(i);
      }
    }
  }

  // どのAABBとも当たっていなければリターン
  if (hitGroups.empty()) {
    return false;
  }

  // レイキャストの最終結果
  bool isHit = false;

  // レイのベクトル
  DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalEnd, LocalStart);
  // ワールド空間のレイの長さ
  if (hitResult != nullptr) {
    DirectX::XMStoreFloat(&hitResult->distance, DirectX::XMVector3Length(LocalRayVec));
  }
  // 正規化
  LocalRayVec = DirectX::XMVector3Normalize(LocalRayVec);

  // 当たっていたAABBの情報を基にレイキャスト
  while (!hitGroups.empty()) {
    int groupIndex = hitGroups.front();
    hitGroups.pop();

    // AABBに含まれる頂点群からレイに当たっているポリゴンを調べる
    for (auto& vertexData : groups[groupIndex].vertexDatas) {
      // ポリゴンとレイの当たり判定
      {
        // ポリゴンを構成する3頂点
        DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&vertices[vertexData.keyID][vertexData.vertexA].position);
        DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&vertices[vertexData.keyID][vertexData.vertexA + 1].position);
        DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&vertices[vertexData.keyID][vertexData.vertexA + 2].position);

        // 三角形の三辺ベクトルを算出
        DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
        DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
        DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

        // 三角形の法線ベクトルを算出
        DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

        // 内積の結果がプラスならば裏向き
        DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayVec, N);
        // 裏ポリゴンなら無視
        if (DirectX::XMVectorGetX(Dot) >= 0) continue;

        // レイと平面の交点を算出
        DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, LocalStart);
        DirectX::XMVECTOR D1 = DirectX::XMVector3Dot(SA, N);
        DirectX::XMVECTOR D2 = Dot;

        DirectX::XMVECTOR X = DirectX::XMVectorDivide(D1, D2);
        float x = DirectX::XMVectorGetX(X);
        // 交点までの距離が今までに計算した最近距離より大きい時はスキップ
        if (x < 0.0f || x > hitResult->distance) continue;
        DirectX::XMVECTOR P = DirectX::XMVectorAdd(LocalStart, DirectX::XMVectorScale(LocalRayVec, x));

        // 交点が三角形の内側にあるか判定
        // 1つめ
        DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
        DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
        DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
        float dot1;
        DirectX::XMStoreFloat(&dot1, Dot1);
        if (dot1 < 0)continue;

        // 2つめ
        DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
        DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
        DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
        float dot2;
        DirectX::XMStoreFloat(&dot2, Dot2);
        if (dot2 < 0)continue;

        // 3つめ
        DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
        DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
        DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
        float dot3;
        DirectX::XMStoreFloat(&dot3, Dot3);
        if (dot3 < 0)continue;

        // このポリゴンと当たってた
        if (hitResult == nullptr)return true;

        // HitResultの更新
        hitResult->distance = x;
        DirectX::XMStoreFloat3(&hitResult->position, P);
        DirectX::XMStoreFloat3(&hitResult->normal, N);
        isHit = true;

#ifdef DRAW_POLYGON_GROUP
        line->AddVertex(vertices[vertexData.keyID][vertexData.vertexA].position, { 0,1,0,1 });
        line->AddVertex(vertices[vertexData.keyID][vertexData.vertexA + 1].position, { 0,1,0,1 });
        line->AddVertex(vertices[vertexData.keyID][vertexData.vertexA + 1].position, { 0,1,0,1 });
        line->AddVertex(vertices[vertexData.keyID][vertexData.vertexA + 2].position, { 0,1,0,1 });
        line->AddVertex(vertices[vertexData.keyID][vertexData.vertexA + 2].position, { 0,1,0,1 });
        line->AddVertex(vertices[vertexData.keyID][vertexData.vertexA].position, { 0,1,0,1 });
#endif
      }
    }
  }

  // どこかのポリゴンに当たっていたら
  if (isHit) {
    // ローカル空間からワールド空間に変換
    DirectX::XMStoreFloat3(&hitResult->position, DirectX::XMVector3TransformCoord(
      DirectX::XMLoadFloat3(&hitResult->position), WorldMat));

    DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hitResult->normal);
    Normal = DirectX::XMVector3Normalize(Normal);
    DirectX::XMStoreFloat3(&hitResult->normal, DirectX::XMVector3TransformNormal(
      Normal, WorldMat));

    hitResult->distance = sqrtf(Mathf::Dot(hitResult->position - start));
  }

  return isHit;


  return false;
}

bool RayCastManager::RayVsBox(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& positive, const DirectX::XMFLOAT3& negative)
{
  float min[] = {
    negative.x,
    negative.y,
    negative.z
  };
  float max[] = {
    positive.x,
    positive.y,
    positive.z
  };

  float pos[] = { start.x, start.y, start.z };
  float dir[] = { end.x - start.x, end.y - start.y, end.z - start.z };

  float t_min = 0.0f;
  float t_max = 1.0f;

  for (int i = 0; i < 3; ++i) {
    if (abs(dir[i]) < FLT_EPSILON) {
      if (pos[i] < min[i] || pos[i] > max[i])
        return false; // 交差していない
    }
    else {
      float t1 = (min[i] - pos[i]) / dir[i];
      float t2 = (max[i] - pos[i]) / dir[i];

      if (dir[i] < 0.0f) {
        float tmp = t1;
        t1 = t2;
        t2 = tmp;
      }

      t_min = max(t_min, t1);
      t_max = min(t_max, t2);

      if (t_min > t_max)
        return false; // 交差していない
    }
  }

  return true;
}
