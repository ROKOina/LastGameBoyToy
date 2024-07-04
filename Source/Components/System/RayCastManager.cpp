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

  // �X�^�[�g�ƌ�_�̋�����ۑ�����
  float length = FLT_MAX;

  // ��ԋ߂���_�̏��
  Result nearHit = {};

  for (int i = 0; i < collisionModels.size(); ++i)
  {
    // ���C�L���X�g
    if (Collide(start, end, collisionModels[i], resultPtr)) {
      if (positionPtr == nullptr && resultPtr == nullptr && objectPtr == nullptr)return true;

      isHit = true;

      //  ��_�ԋ��������߂�
      DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
      DirectX::XMVECTOR Point = DirectX::XMLoadFloat3(&resultPtr->position);
      DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Point, Start);

      float vecLength = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

      // ���܂ł̋������Z����Α��
      if (vecLength < length)
      {
        // �ŒZ�����̍X�V
        length = vecLength;

        // �ŒZ�����̃q�b�g�����X�V
        nearHit = *resultPtr;
      }
    }
  }

  // ������
  if (positionPtr)*positionPtr = nearHit.position;
  if (objectPtr)*objectPtr = nearHit.hitObject;
  if (resultPtr) {
    *resultPtr = nearHit;

    // �X�Η��̌v�Z
    nearHit.normal.y = 0;
    float horizon = sqrtf(Mathf::Dot(nearHit.normal));

    // �l���P�ɋ߂��قǕǂɂȂ�
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

  // ���C�����g�𒆐S�Ƃ�����Ԃɕϊ�
  DirectX::XMVECTOR LocalStart = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&start), InverseTransform);
  DirectX::XMVECTOR LocalEnd = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&end), InverseTransform);

  // AABB�Ƃ̓����蔻��
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

  // �ǂ�AABB�Ƃ��������Ă��Ȃ���΃��^�[��
  if (hitGroups.empty()) {
    return false;
  }

  // ���C�L���X�g�̍ŏI����
  bool isHit = false;

  // ���C�̃x�N�g��
  DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalEnd, LocalStart);
  // ���[���h��Ԃ̃��C�̒���
  if (hitResult != nullptr) {
    DirectX::XMStoreFloat(&hitResult->distance, DirectX::XMVector3Length(LocalRayVec));
  }
  // ���K��
  LocalRayVec = DirectX::XMVector3Normalize(LocalRayVec);

  // �������Ă���AABB�̏�����Ƀ��C�L���X�g
  while (!hitGroups.empty()) {
    int groupIndex = hitGroups.front();
    hitGroups.pop();

    // AABB�Ɋ܂܂�钸�_�Q���烌�C�ɓ������Ă���|���S���𒲂ׂ�
    for (auto& vertexData : groups[groupIndex].vertexDatas) {
      // �|���S���ƃ��C�̓����蔻��
      {
        // �|���S�����\������3���_
        DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&vertices[vertexData.keyID][vertexData.vertexA].position);
        DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&vertices[vertexData.keyID][vertexData.vertexA + 1].position);
        DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&vertices[vertexData.keyID][vertexData.vertexA + 2].position);

        // �O�p�`�̎O�Ӄx�N�g�����Z�o
        DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
        DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
        DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

        // �O�p�`�̖@���x�N�g�����Z�o
        DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

        // ���ς̌��ʂ��v���X�Ȃ�Η�����
        DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayVec, N);
        // ���|���S���Ȃ疳��
        if (DirectX::XMVectorGetX(Dot) >= 0) continue;

        // ���C�ƕ��ʂ̌�_���Z�o
        DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, LocalStart);
        DirectX::XMVECTOR D1 = DirectX::XMVector3Dot(SA, N);
        DirectX::XMVECTOR D2 = Dot;

        DirectX::XMVECTOR X = DirectX::XMVectorDivide(D1, D2);
        float x = DirectX::XMVectorGetX(X);
        // ��_�܂ł̋��������܂łɌv�Z�����ŋߋ������傫�����̓X�L�b�v
        if (x < 0.0f || x > hitResult->distance) continue;
        DirectX::XMVECTOR P = DirectX::XMVectorAdd(LocalStart, DirectX::XMVectorScale(LocalRayVec, x));

        // ��_���O�p�`�̓����ɂ��邩����
        // 1��
        DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
        DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
        DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
        float dot1;
        DirectX::XMStoreFloat(&dot1, Dot1);
        if (dot1 < 0)continue;

        // 2��
        DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
        DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
        DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
        float dot2;
        DirectX::XMStoreFloat(&dot2, Dot2);
        if (dot2 < 0)continue;

        // 3��
        DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
        DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
        DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
        float dot3;
        DirectX::XMStoreFloat(&dot3, Dot3);
        if (dot3 < 0)continue;

        // ���̃|���S���Ɠ������Ă�
        if (hitResult == nullptr)return true;

        // HitResult�̍X�V
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

  // �ǂ����̃|���S���ɓ������Ă�����
  if (isHit) {
    // ���[�J����Ԃ��烏�[���h��Ԃɕϊ�
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
        return false; // �������Ă��Ȃ�
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
        return false; // �������Ă��Ȃ�
    }
  }

  return true;
}
