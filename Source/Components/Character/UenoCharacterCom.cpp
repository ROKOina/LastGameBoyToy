#include "UenoCharacterCom.h"
#include "State\BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "Components/Character/State/UenoCharacterState.h"
#include "../CameraCom.h"
#include "Components/CPUParticle.h"
#include "Components\RendererCom.h"

//������
UenoCharacterCom::UenoCharacterCom()
{
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("lazer");
        obj->AddComponent<CPUParticle>("Data\\Effect\\lazer.cpuparticle", 1000);
    }

    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("s");
        gpulazerparticle = obj->AddComponent<GPUParticle>("Data\\Effect\\lazer.gpuparticle", 10000);
    }
}

//������
void UenoCharacterCom::Start()
{
    //�X�e�[�g�o�^
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<UenoCharacterState_AttackState>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::MOVE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

//�X�V����
void UenoCharacterCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);
    LazerParticleUpdate(elapsedTime);
}

//imgui
void UenoCharacterCom::OnGUI()
{
    CharacterCom::OnGUI();
}

//���C���A�^�b�N
void UenoCharacterCom::MainAttackDown()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}

//�p�[�e�B�N���X�V
void UenoCharacterCom::LazerParticleUpdate(float elapsedTime)
{
    //�ʒu�̔������ƃp�[�e�B�N���̔��˃x�N�g��
    DirectX::XMFLOAT3 pos = { GetGameObject()->transform_->GetWorldPosition().x,GetGameObject()->transform_->GetWorldPosition().y + 1,GetGameObject()->transform_->GetWorldPosition().z };
    gpulazerparticle->GetGameObject()->transform_->SetWorldPosition(pos);
    gpulazerparticle->m_gpu->data.direction = GetGameObject()->transform_->GetWorldFront();

    //�t���O��ONOFF�𐧌�
    gpulazerparticle->m_gpu->data.loop = lazerflag;
}