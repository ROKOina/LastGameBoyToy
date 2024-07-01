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
    stateMachine.AddState(CHARACTER_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::ATTACK, std::make_shared<UenoCharacterState_AttackState>(this));

    stateMachine.ChangeState(CHARACTER_ACTIONS::MOVE);
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
void UenoCharacterCom::MainAttack()
{
    stateMachine.ChangeState(CHARACTER_ACTIONS::ATTACK);
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