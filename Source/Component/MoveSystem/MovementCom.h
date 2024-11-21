#pragma once

#include "Component/System/Component.h"

#define GRAVITY_NORMAL 0.98

//Movementクラス
class MovementCom : public Component
{
    //コンポーネントオーバーライド
public:
    MovementCom() {}
    ~MovementCom() {}

    // 名前取得
    const char* GetName() const override { return "Movement"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    //velocity
    void AddForce(const DirectX::XMFLOAT3& force);

    //ランダム方向に飛ばす
    void ApplyRandomForce(float forcestrength, float yforce);

    //nonMaxSpeedVelocity
    void AddNonMaxSpeedForce(const DirectX::XMFLOAT3& force);

private:

    //縦方向移動更新
    void VerticalUpdate(float elapsedTime);

    //横方向移動更新
    void HorizonUpdate(float elapsedTime);

    //速力を更新( 垂直方向 )
    void VelocityApplyPositionVertical(float elapsedTime, const float& moveVec);

    //速力を更新( 水平方向 )
    void VelocityApplyPositionHorizontal(float elapsedTime, const DirectX::XMFLOAT3& moveVec);

public:

#pragma ゲッターとセッター
    //velocity
    void ZeroVelocity() { velocity_ = { 0,0,0 }; }
    void ZeroVelocityY() { velocity_ = { velocity_.x,0,velocity_.z }; }
    const DirectX::XMFLOAT3& GetVelocity()const { return velocity_; }
    void SetVelocity(DirectX::XMFLOAT3 velo) { velocity_ = velo; }

    //addnonmaxvelocity
    void ZeroNonMaxSpeedVelocity() { nonMaxSpeedVelocity_ = { 0,0,0 }; }
    const DirectX::XMFLOAT3& GetNonMaxSpeedVelocity()const { return nonMaxSpeedVelocity_; }
    void SetNonMaxSpeedVelocity(DirectX::XMFLOAT3 velo) { nonMaxSpeedVelocity_ = velo; }

    //重力
    const float& GetGravity()const { return gravity_; }
    void SetGravity(float gravity) { gravity_ = gravity; }

    //重力影響度
    const float& GetGravityEffect()const { return gravityeffect; }
    void SetGravityEffect(float gravityeffect) { this->gravityeffect = gravityeffect; }

    //落下スピード
    const float& GetFallSpeed()const { return fallspeed; }
    void SetFallSpeed(float fallspeed) { this->fallspeed = fallspeed; }

    //接地判定
    const bool OnGround() { return onGround_; }
    void SetOnGround(bool flag) { onGround_ = flag; }

    // 着地した瞬間のフラグを取得
    bool JustLanded() const { return justLanded_; }

    //摩擦
    const float& GetFriction()const { return friction_; }
    void SetFriction(float friction) { friction_ = friction; }

    //最大速度
    const float& GetFisrtMoveMaxSpeed()const { return firstMoveMaxSpeed; }  //初期最大速度（基準）
    const float& GetMoveMaxSpeed()const { return moveMaxSpeed_; }
    void SetMoveMaxSpeed(float moveMaxSpeed) { moveMaxSpeed = moveMaxSpeed_; }
    void SetAddMoveMaxSpeed(float addSpeed) { moveMaxSpeed_ += addSpeed; }  //足す
    void SetSubMoveMaxSpeed(float subSpeed) { moveMaxSpeed_ -= subSpeed; }  //引く

    //加速度
    const float& GetMoveAcceleration()const { return moveAcceleration_; }
    void SetMoveAcceleration(float moveAcceleration) { moveAcceleration_ = moveAcceleration; }

    //レイキャスト使用
    const bool& GetIsRaycast()const { return isRaycast; }
    void SetIsRaycast(float isRaycast) { this->isRaycast = isRaycast; }

#pragma endregion

private:
    DirectX::XMFLOAT3 velocity_ = { 0,0,0 };   //速力
    DirectX::XMFLOAT3 nonMaxSpeedVelocity_ = { 0,0,0 }; //最大スピードを無視した速力
    float gravity_ = GRAVITY_NORMAL;           //重力
    float gravityeffect = 30.0f;               //重力の影響度
    float fallspeed = -100.0f;                 //落ちるスピード
    bool onGround_ = false;                    //地面についているか
    bool wasOnGround_ = false;                 // 前フレームの着地状態
    bool justLanded_ = false;                  // 今フレームで着地した瞬間かどうか
    float friction_ = 12.620f;                 //摩擦
    float moveMaxSpeed_ = 8.0f;                //最大速度
    float firstMoveMaxSpeed = 8.0f;             //初期最大速度
    float moveAcceleration_ = 3.0f;            //加速度
    inline static float stepOffset = 0.5f;     //レイキャスト用のオフセット
    bool isRaycast = true;                      //レイキャストをするか（true：使用する）
};