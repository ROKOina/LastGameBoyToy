#pragma once
#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include <Component/Audio/AudioCom.h>
#include <Component/Character/CharaStatusCom.h>

class ScarecrowCom;


//トレーニングルームの案山子

class Scarecrow_BaseState :public State<ScarecrowCom>
{
public:

    Scarecrow_BaseState(ScarecrowCom* owner);

    void RandomMove(float moveSpeed);
    //ルール
    //メモ帳に書くとなくすからここに書きます
    //ランダムで右左に行って制限値を超えたら　ゼッタイ逆向きに進むようにする

    DirectX::XMFLOAT3 randomPos = { 0,0,0 };
    DirectX::XMFLOAT3 VEC = { 0,0,0 };
    
    float limitRightX = -28.0f;
    float limitLeftX = -10.0f;
    float moveSpeed = 2.0f;

    
    // 0=右 1=左
    int rightleftIndex = 0;

protected:
    std::weak_ptr<ScarecrowCom> scarecrowCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
    std::weak_ptr<AudioCom> audioCom;
    std::weak_ptr<CharaStatusCom>characterstatas;




    
};


//待機
class Scarecrow_IdleState :public Scarecrow_BaseState
{
public:
    Scarecrow_IdleState(ScarecrowCom* owner) :Scarecrow_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Idle"; }
};

//ランダム移動後の待機
class Scarecrow_RandomIdleState :public Scarecrow_BaseState
{
public:
    Scarecrow_RandomIdleState(ScarecrowCom*owner):Scarecrow_BaseState(owner){}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "RandomIdle"; }

private:
    float randomIdleTime = 0;
    float randomIdleTimer = 0;

};

//移動
class Scarecrow_MoveState :public Scarecrow_BaseState
{
public:
    Scarecrow_MoveState(ScarecrowCom* owner):Scarecrow_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    //void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Move"; }

};


//死亡
class Scarecrow_DeathState :public Scarecrow_BaseState
{
public:
    Scarecrow_DeathState(ScarecrowCom* owner) :Scarecrow_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    //void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Death"; }

};
