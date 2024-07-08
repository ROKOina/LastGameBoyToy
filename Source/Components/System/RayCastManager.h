#pragma once

#include <vector>
#include <memory>
#include <DirectXMath.h>

class GameObject;
class RayCollisionCom;

// ���C�L���X�g�}�l�[�W���[( �V���O���g�� )
class RayCastManager
{
private:
  RayCastManager() { }
  ~RayCastManager() {}

public:
  // ���C�L���X�g�̌��ʏ��
  struct Result
  {
    DirectX::XMFLOAT3 position = { 0,0,0 }; // ���C�ƃ|���S���̌�_
    DirectX::XMFLOAT3 normal = { 0,0,0 };   // �Փ˂����|���S���̖@���x�N�g��
    float distance = 0.0f;                  // ���C�̎n�_�����_�܂ł̋���
    int materialIndex = -1;                 // �Փ˂����|���S���̃}�e���A���ԍ�
    float slopeRate = 0.0f;                 // ��_�̌X�Η�
    std::weak_ptr<GameObject> hitObject;    // ���������I�u�W�F�N�g
  };

public:
  // �C���X�^���X�擾
  static RayCastManager& Instance()
  {
    static RayCastManager instance;
    return instance;
  }

  // �o�^
  void Register(std::shared_ptr<RayCollisionCom> newCollision) { collisionModels.emplace_back(newCollision); }

  // ���O
  void Remove(std::shared_ptr<RayCollisionCom> removeCollision);

  // ���� ( ������������̂ݎ擾 )
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end) { return RayCast(start, end, nullptr, nullptr, nullptr); };

  // ���� ( ���������ʒu�̂ݎ擾 )
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, DirectX::XMFLOAT3& position) { return RayCast(start, end, &position, nullptr, nullptr); };

  // ���� ( ���������I�u�W�F�N�g�̂ݎ擾 )
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, std::weak_ptr<GameObject> hitObject) { return RayCast(start, end, nullptr, nullptr, &hitObject); };

  // ���� ( �������������擾 )
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, Result& result) { return RayCast(start, end, &result.position, &result, &result.hitObject); };

private:
  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, DirectX::XMFLOAT3* positionPtr, Result* resultPtr, std::weak_ptr<GameObject>* objectPtr);

  bool Collide(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, std::weak_ptr<RayCollisionCom> collision, Result* hitResult);

  bool RayVsBox(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& positive, const DirectX::XMFLOAT3& negative);

private:
  std::vector<std::weak_ptr<RayCollisionCom>> collisionModels;

};
