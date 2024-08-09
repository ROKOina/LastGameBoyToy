#include "UenoCharacterCom.h"
#include "State\BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "Components/Character/State/UenoCharacterState.h"
#include "../CameraCom.h"
#include "Components/CPUParticle.h"
#include "Components\RendererCom.h"

//初期化
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

//初期化
void UenoCharacterCom::Start()
{
    //ステート登録
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<UenoCharacterState_AttackState>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::MOVE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

//更新処理
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

//メインアタック
void UenoCharacterCom::MainAttackDown()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}

//パーティクル更新
void UenoCharacterCom::LazerParticleUpdate(float elapsedTime)
{
    //位置の微調整とパーティクルの発射ベクトル
    DirectX::XMFLOAT3 pos = { GetGameObject()->transform_->GetWorldPosition().x,GetGameObject()->transform_->GetWorldPosition().y + 1,GetGameObject()->transform_->GetWorldPosition().z };
    gpulazerparticle->GetGameObject()->transform_->SetWorldPosition(pos);
    gpulazerparticle->m_gpu->data.direction = GetGameObject()->transform_->GetWorldFront();

    //フラグでONOFFを制御
    gpulazerparticle->m_gpu->data.loop = lazerflag;
}