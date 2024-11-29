#include "TrainingScarecrowState.h"
#include "Component/Enemy/ScarecrowCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\System\SpawnCom.h"
#include "Component\PostEffect\PostEffect.h"


//基底コンストラクタ
Scarecrow_BaseState::Scarecrow_BaseState(ScarecrowCom* owner) :State(owner)
{
    scarecrowCom=owner->GetGameObject()->GetComponent<ScarecrowCom>();
    moveCom=owner->GetGameObject()->GetComponent<MovementCom>();
    transCom = owner->GetGameObject()->GetComponent<TransformCom>();
    animationCom = owner->GetGameObject()->GetComponent<AnimationCom>();
    audioCom = owner->GetGameObject()->GetComponent<AudioCom>();
    characterstatas = owner->GetGameObject()->GetComponent<CharaStatusCom>();
}

void Scarecrow_BaseState::RandomMove(float moveSpeed)
{
    float posX = transCom.lock()->GetWorldPosition().x;

    randomPos.x = Mathf::RandomRange(limitLeftX, limitRightX);
    randomPos.y = 0.0f;
    randomPos.z = transCom.lock()->GetWorldPosition().z;
}

#pragma region 待機
void Scarecrow_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, false, 0.1f);
}

void Scarecrow_IdleState::Execute(const float& elapsedTime)
{
    //モード切り替え
    if (owner->GetCrowMode() == 1)
    {
        scarecrowCom.lock()->GetStateMachine().ChangeState(ScarecrowCom::ScareCrowState::MOVE);
    }

    //死亡
    if (characterstatas.lock()->IsDeath())
    {
        scarecrowCom.lock()->GetStateMachine().ChangeState(ScarecrowCom::ScareCrowState::DEATH);
    }
}
#pragma endregion

#pragma region ランダム待機
void Scarecrow_RandomIdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, false, 0.1f);
    randomIdleTime = Mathf::RandomRange(0.5f, 1.5f);
}

void Scarecrow_RandomIdleState::Execute(const float& elapsedTime)
{
    randomIdleTimer += elapsedTime;
    if (randomIdleTime < randomIdleTimer)
    {
        scarecrowCom.lock()->GetStateMachine().ChangeState(ScarecrowCom::ScareCrowState::MOVE);
    }
}
#pragma endregion

#pragma region 移動
void Scarecrow_MoveState::Enter()
{
    RandomMove(1.2f);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_walk_front"), true, false, 0.1f);
}

void Scarecrow_MoveState::Execute(const float& elapsedTime)
{

    moveCom.lock()->AddForce(randomPos * moveSpeed);


    if (randomPos.x == transCom.lock()->GetWorldPosition().x)
    {
        scarecrowCom.lock()->GetStateMachine().ChangeState(ScarecrowCom::ScareCrowState::RANDOMIDLE);
    }


    //死亡
    if (characterstatas.lock()->IsDeath())
    {
        scarecrowCom.lock()->GetStateMachine().ChangeState(ScarecrowCom::ScareCrowState::DEATH);
    }
}


#pragma region 死亡
void Scarecrow_DeathState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_dead"), false, false, 0.1f);
}

void Scarecrow_DeathState::Execute(const float& elapsedTime)
{

}

