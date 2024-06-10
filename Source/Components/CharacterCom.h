#pragma once
#include "System\Component.h"

class CharacterCom : public Component
{
public:
    CharacterCom() {};
    ~CharacterCom() override {};

    // 名前取得
    const char* GetName() const override { return "Character"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

    // コントローラーから進行方向取得
    DirectX::XMFLOAT3 CalcMoveVec() const;

    
};

class StateMachine
{

};
class State
{

};

class Chara1Statemachine : public StateMachine
{
    State jamp
};
