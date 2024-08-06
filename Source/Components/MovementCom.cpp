#include "MovementCom.h"

#include <imgui.h>
#include "GameSource/Math/Mathf.h"
#include "TransformCom.h"
#include "System/RayCastManager.h"
#include "Graphics\Graphics.h"

// 開始処理
void MovementCom::Start()
{
}

// 更新処理
void MovementCom::Update(float elapsedTime)
{
    // 経過フレーム
    float elapsedFrame = 60.0f * elapsedTime;

    //縦方向移動更新
    VerticalUpdate(elapsedFrame);

    HorizonUpdate(elapsedTime);

    //世界のスピード
    float worldSpeed = Graphics::Instance().GetWorldSpeed();
    //オブジェクトのスピード
    float objSpeed = GetGameObject()->GetObjSpeed();

    // 移動量
    DirectX::XMFLOAT3 velocity;
    velocity.x = (velocity_.x + nonMaxSpeedVelocity_.x) * (elapsedTime * worldSpeed * objSpeed);
    velocity.y = (velocity_.y + nonMaxSpeedVelocity_.y) * (elapsedTime * worldSpeed * objSpeed);
    velocity.z = (velocity_.z + nonMaxSpeedVelocity_.z) * (elapsedTime * worldSpeed * objSpeed);

    VelocityApplyPositionHorizontal(elapsedTime, velocity);
    VelocityApplyPositionVertical(elapsedTime, velocity.y);
};

// GUI描画
void MovementCom::OnGUI()
{
    ImGui::DragFloat3("velocity", &velocity_.x);
    ImGui::DragFloat3("nonMaxSpeedVelocity_", &nonMaxSpeedVelocity_.x);
    ImGui::DragFloat("gravity", &gravity_, 0.1f);
    ImGui::DragFloat("gravityeffect", &gravityeffect, 0.1f);
    ImGui::DragFloat("friction", &friction_, 0.1f);
    ImGui::DragFloat("moveMaxSpeed", &moveMaxSpeed_, 0.1f);
    ImGui::DragFloat("moveAcceleration", &moveAcceleration_, 0.1f);
    ImGui::Checkbox("onGround", &onGround_);
}

//縦方向移動更新
void MovementCom::VerticalUpdate(float elapsedTime)
{
    velocity_.y -= gravity_ * gravityeffect * elapsedTime;
    velocity_.y = (std::max)(velocity_.y, -15.0f);
}

void MovementCom::HorizonUpdate(float elapsedTime)
{
    DirectX::XMFLOAT3 horizonVelocity = { velocity_.x,0,velocity_.z };
    DirectX::XMVECTOR HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    float horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    //最大速度を超えている場合は制限する
    if (horiLengthSq > moveMaxSpeed_ * moveMaxSpeed_)
    {
        DirectX::XMVECTOR MaxSpeed = DirectX::XMVectorScale(DirectX::XMVector3Normalize(HorizonVelocity), moveMaxSpeed_);
        DirectX::XMFLOAT3 newMaxVelocity;
        DirectX::XMStoreFloat3(&newMaxVelocity, MaxSpeed);
        velocity_.x = newMaxVelocity.x;
        velocity_.z = newMaxVelocity.z;
    }

    horizonVelocity = { velocity_.x,0,velocity_.z };
    HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    // 摩擦力
    float friction;
    {
        //世界のスピード
        float worldSpeed = Graphics::Instance().GetWorldSpeed();
        //オブジェクトのスピード
        float objSpeed = GetGameObject()->GetObjSpeed();
        friction = friction_ * (elapsedTime * worldSpeed * objSpeed);
    }

    // 摩擦力
    if (horiLengthSq > 0.0f)
    {
        DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(DirectX::XMVectorScale(HorizonVelocity, -1), friction);
        DirectX::XMStoreFloat3(&velocity_, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&velocity_), FriVelocity));
    }
    else
    {
        velocity_.x = 0;
        velocity_.z = 0;
    }

    //最大速度の無い摩擦計算
    horizonVelocity = { nonMaxSpeedVelocity_.x,0,nonMaxSpeedVelocity_.z };
    HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    //摩擦力
    if (horiLengthSq > 0.0f)
    {
        DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(DirectX::XMVectorScale(HorizonVelocity, -1), friction);
        DirectX::XMFLOAT3 newVelocity;
        DirectX::XMStoreFloat3(&newVelocity, FriVelocity);
        AddNonMaxSpeedForce(newVelocity);
    }
    else
    {
        nonMaxSpeedVelocity_.x = 0;
        nonMaxSpeedVelocity_.z = 0;
    }
}

//速力を更新
void MovementCom::VelocityApplyPositionVertical(float elapsedTime, const float& moveVec)
{
    DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();

    // 地面との着地判定
    {
        // レイキャスト用のオフセット
        TransformCom* transform = GetGameObject()->transform_.get();

        // レイの開始位置は足元より少し上
        DirectX::XMFLOAT3 start = transform->GetWorldPosition();
        start.y += stepOffset;

        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = transform->GetWorldPosition();
        end.y += moveVec;

        // 判定
        RayCastManager::Result hit;
        if (RayCastManager::Instance().RayCast(start, end, hit)) {
            // 地面に接地している

#if 1 // 傾斜率が高いと滑り落ちる処理

      // 傾斜率が閾値以上の場合、法線方向に押し出す
            if (hit.slopeRate >= slopeThreshold) {
                // 世界のスピード
                float worldSpeed = Graphics::Instance().GetWorldSpeed();

                // 傾斜率に応じて滑る速度を変化させる
                float slip = max(slipPower * hit.slopeRate, 1.0f);
                position += hit.normal * slip * elapsedTime * worldSpeed;

                // 押し出した場所が埋まっていた場合
                RayCastManager::Result hit2;
                if (RayCastManager::Instance().RayCast(start, end, hit2))
                {
                    position.y = hit2.position.y;
                }
            }
            else
                position.y = hit.position.y;

#else // 滑り落ちたりせずに位置を反映させる処理
            position.y = hit.position.y;

#endif // 1

            velocity_.y = 0;
            nonMaxSpeedVelocity_.y = 0;
            onGround_ = true;
        }
        else {
            position.y += moveVec;
            onGround_ = false;
        }
    }

    GetGameObject()->transform_->SetWorldPosition(position);
}

void MovementCom::VelocityApplyPositionHorizontal(float elapsedTime, const DirectX::XMFLOAT3& moveVec)
{
    DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();

    // 水平速力量計算
    DirectX::XMFLOAT2 Speed = {
      moveVec.x,
      moveVec.z
    };
    float velocityLengthXZ = Mathf::Dot(Speed, Speed);

    if (velocityLengthXZ > 0.0f)
    {
        // レイの始点位置と終点位置
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        DirectX::XMFLOAT3 end = { position.x + moveVec.x, position.y + stepOffset, position.z + moveVec.z };

        // レイキャストによる壁判定
        RayCastManager::Result hit;
        if (RayCastManager::Instance().RayCast(start, end, hit))
        {
            // 移動後の位置から壁までのベクトル
            DirectX::XMVECTOR Start = XMLoadFloat3(&start);
            DirectX::XMVECTOR End = XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Start, End);

            // 壁の法線
            DirectX::XMVECTOR Normal = XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線ベクトルに射影
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);

            // 補正位置の計算
            DirectX::XMFLOAT3 correction;
            DirectX::XMStoreFloat3(&correction, DirectX::XMVectorMultiplyAdd(Normal, Dot, End));

            start = hit.position;
            end = correction;

            // 補正後の位置が壁にめり込んでいたら
            RayCastManager::Result hit2;
            if (RayCastManager::Instance().RayCast(start, end, hit2))
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
            else
            {
                position.x = correction.x;
                position.z = correction.z;
            }
        }
        else
        {
            position.x += moveVec.x;
            position.z += moveVec.z;
        }
    }

    GetGameObject()->transform_->SetWorldPosition(position);
}