#pragma once

#include <DirectXMath.h>
#include "System\Component.h"

// ��������p�R���|�[�l���g�̐����Ɏg�p����R���e�L�X�g
struct ProjectileContext
{
  DirectX::XMFLOAT3 velocity = {};  // �ړ�����x�N�g��

  float acceleration    = 0.0f;     // �����x
  float maxSpeed        = 0.0f;     // �ő呬�x
  float restitution     = 0.8f;		  // �����W��
  float frictionGround  = 1;        // �n�ʂƂ̖��C
  float frictionAir     = 1;        // ��C��R

  bool isApplyGravity   = true;     // �d�͂̉e�����󂯂邩
  bool isCollideTerrain = true;     // �n�`�Ƃ̓����蔻������邩
  bool isHitOnce        = false;    // ��x�����������_�ňړ����~�߂邩 ( �n�`�Ƃ̓����蔻�� )
};

// �e�E�O���l�[�h�ȂǓ��˕��p�̋�������R���|�[�l���g
class ProjectileCom :public Component
{
public:
  ProjectileCom(const ProjectileContext& context) :context(context) {}

  // ���O�擾
  const char* GetName() const override { return "ProjectileCom"; }

  // �X�V����
  void Update(float elapsedTime)override;

  // TRUE = �n�`�Ɠ�������
  bool OnHitTerrain() { return onHitTerrain; }
  bool OnGround() { return onGround; }

private:
  // �n�`�ɓ����������Ƀo�E���h����
  void Rebound(const DirectX::XMFLOAT3& normal);

  // �n�ʂƂ̓����蔻��
  void CheckHitGround();

  // ���C�͂̍X�V
  void UpdateFriction(float elapsedTime);

  // �d�͂̍X�V
  void UpdateGravity(float elapsedTime);

  // �������ړ��X�V
  void HorizonUpdate(float elapsedTime);

  // ���͂𔽉f
  void ApplyVelocity(float elapsedTime);

private:
  const float MASS = 1.0f;          // ����
  const float MAX_GRAVITY = 10.0f;  // �ő�d�͉����x
  const float STEP_OFFSET = 0.5f;   // ���C�L���X�g�p�̃I�t�Z�b�g�l

  float gravitySimulate = 0.0f;
  ProjectileContext context;

  bool onGround   = false;      // �ڒn���Ă��邩
  bool onHitTerrain = false;    // �n�`�ɓ��������u��
};