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
    stateMachine.AddState(CHARACTER_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::ATTACK, std::make_shared<UenoCharacterState_AttackState>(this));

    stateMachine.ChangeState(CHARACTER_ACTIONS::MOVE);
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
void UenoCharacterCom::MainAttack()
{
    stateMachine.ChangeState(CHARACTER_ACTIONS::ATTACK);
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