#pragma once
#include "../../System/Component.h"
#include "../../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"
#include "Input\Input.h"
#include "../../AnimationCom.h"
#include <array>

#include "../../MovementCom.h"

#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();

class NoobEnemyCom : public Component
{
public:
    NoobEnemyCom();
    ~NoobEnemyCom() override;

    //���O�擾
    const char* GetName() const override { return "NoobEnemy"; }

    //GUI�`��
    void OnGUI()override;

    //�J�n����
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

public:

    //�G�l�~�[����v���C���[�ւ̃x�N�g��
    DirectX::XMFLOAT3 GetEnemyToPlayer();

    //�v���C���[�܂ł̋���
    float GetPlayerDist();

private:

    //�X�e�[�g�̓���
    void StateUpdate(float elapsedTime);

private:
    //�X�e�[�g�֘A

    //�J��

    //�ҋ@�X�e�[�g
    void TransitionIdleState();
    //�ǐՃX�e�[�g
    void TransitionPursuit();
    //�����X�e�[�g
    void TransitionExplosion();

    //�X�V

    //�ҋ@�X�e�[�g�X�V����
    void UpdateIdle(float elapsedTime);
    //�ǐՃX�e�[�g�X�V����
    void UpdatePursuit(float elpasedTime);
    //�����X�e�[�g�X�V����
    void UpdateExplosion(float elapsedTime);

private:

    //�G���G�̃X�e�[�g
    enum class State
    {
        Idle,
        Purstuit,
        Explosion
    };

    State state = State::Idle;

private:
    //�G���G�̃p�����[�^�[

    //�ړ��X�s�[�h
    float speed = 0.2f;
    //��������܂ł̋���
    float explosionDist = 1.0f;
    //�����͈�
    float explosionRange = 1.5f;
    //�����ŗ^����_���[�W
    float explosionDamage = 10.0f;
    //�����P�\
    float explosionGrace = 1.0f;
};
