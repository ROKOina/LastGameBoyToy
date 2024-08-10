#include "MovementCom.h"
#include <imgui.h>
#include "GameSource/Math/Mathf.h"
#include "TransformCom.h"
#include "System/RayCastManager.h"
#include "Graphics/Graphics.h"

// 更新処理
void MovementCom::Update(float elapsedTime)
{
    // 横方向移動更新
    HorizonUpdate(elapsedTime);

    // 世界のスピード
    float worldSpeed = Graphics::Instance().GetWorldSpeed();
    // オブジェクトのスピード
    float objSpeed = GetGameObject()->GetObjSpeed();

    // 移動量
    DirectX::XMFLOAT3 velocity;
    velocity.x = (velocity_.x + nonMaxSpeedVelocity_.x) * (worldSpeed * objSpeed);
    velocity.y = (velocity_.y + nonMaxSpeedVelocity_.y) * (worldSpeed * objSpeed);
    velocity.z = (velocity_.z + nonMaxSpeedVelocity_.z) * (worldSpeed * objSpeed);

    VelocityApplyPositionHorizontal(elapsedTime, velocity);

    // 縦方向移動更新
    VerticalUpdate(elapsedTime);

    VelocityApplyPositionVertical(elapsedTime, velocity.y);
}

// GUI描画
void MovementCom::OnGUI()
{
    ImGui::DragFloat3((char*)u8"速力", &velocity_.x, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat((char*)u8"重力", &gravity_, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat((char*)u8"重力影響度", &gravityeffect, 0.01f, 0.0f, 30.0f);
    ImGui::DragFloat((char*)u8"落下スピード", &fallspeed, 0.1f, -100.0f, 0.0f);
    ImGui::DragFloat((char*)u8"摩擦", &friction_, 0.01f, 0.0f, 40.0f);
    ImGui::DragFloat((char*)u8"最大速度", &moveMaxSpeed_, 0.1f, 0.0f, 30.0f);
    ImGui::DragFloat((char*)u8"加速度", &moveAcceleration_, 0.01f, 0.0f, 10.0f);
    ImGui::Checkbox((char*)u8"接地", &onGround_);
}

// 縦方向移動更新
void MovementCom::VerticalUpdate(float elapsedTime)
{
    // 重力による速力の変化
    velocity_.y -= gravity_ * gravityeffect * elapsedTime;
    velocity_.y = (std::max)(velocity_.y, fallspeed);
}

// 横方向移動更新
void MovementCom::HorizonUpdate(float elapsedTime)
{
    DirectX::XMFLOAT3 horizonVelocity = { velocity_.x, 0, velocity_.z };
    DirectX::XMVECTOR HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    float horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    // 最大速度を超えている場合は制限する
    if (horiLengthSq > moveMaxSpeed_ * moveMaxSpeed_)
    {
        DirectX::XMVECTOR MaxSpeed = DirectX::XMVectorScale(DirectX::XMVector3Normalize(HorizonVelocity), moveMaxSpeed_);
        DirectX::XMFLOAT3 newMaxVelocity;
        DirectX::XMStoreFloat3(&newMaxVelocity, MaxSpeed);
        velocity_.x = newMaxVelocity.x;
        velocity_.z = newMaxVelocity.z;
    }

    horizonVelocity = { velocity_.x, 0, velocity_.z };
    HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    // 摩擦力
    float friction = friction_ * (elapsedTime * Graphics::Instance().GetWorldSpeed() * GetGameObject()->GetObjSpeed());

    // 摩擦力適用
    if (horiLengthSq > 0.0f)
    {
        DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(HorizonVelocity, -friction);
        DirectX::XMStoreFloat3(&velocity_, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&velocity_), FriVelocity));
    }
    else
    {
        velocity_.x = 0;
        velocity_.z = 0;
    }

    // 最大速度の無い摩擦計算
    horizonVelocity = { nonMaxSpeedVelocity_.x, 0, nonMaxSpeedVelocity_.z };
    HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    // 摩擦力
    if (horiLengthSq > 0.0f)
    {
        DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(HorizonVelocity, -friction);
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

// 速力を更新 (垂直方向)
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
        end.y += moveVec - 0.1f;

        // 判定
        RayCastManager::Result hit;
        if (RayCastManager::Instance().RayCast(start, end, hit))
        {
            // 地面に接地している
            position.y = hit.position.y;
            velocity_.y = 0;
            nonMaxSpeedVelocity_.y = 0;
            onGround_ = true;
        }
        else
        {
            position.y += moveVec;
            onGround_ = false;
        }
    }

    GetGameObject()->transform_->SetWorldPosition(position);
}

// 速力を更新 (水平方向)
void MovementCom::VelocityApplyPositionHorizontal(float elapsedTime, const DirectX::XMFLOAT3& moveVec)
{
    DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();

    // 水平速力量計算
    DirectX::XMFLOAT3 totalVelocity = {
        velocity_.x + nonMaxSpeedVelocity_.x,
        velocity_.y,
        velocity_.z + nonMaxSpeedVelocity_.z
    };

    DirectX::XMFLOAT2 Speed = { totalVelocity.x, totalVelocity.z };
    float velocityLengthXZ = Mathf::Dot(Speed, Speed);

    if (velocityLengthXZ > 0.0f)
    {
        // レイの始点位置と終点位置
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        DirectX::XMFLOAT3 end = { position.x + totalVelocity.x * elapsedTime, position.y + stepOffset, position.z + totalVelocity.z * elapsedTime };

        // レイキャストによる壁判定
        RayCastManager::Result hit;
        if (RayCastManager::Instance().RayCast(start, end, hit))
        {
            // 移動後の位置から壁までのベクトル
            DirectX::XMVECTOR Start = XMLoadFloat3(&start);
            DirectX::XMVECTOR End = XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線ベクトルに射影
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);

            // 補正位置の計算
            DirectX::XMFLOAT3 correction;
            DirectX::XMStoreFloat3(&correction, DirectX::XMVectorMultiplyAdd(Normal, Dot, Start));

            // 壁との接触位置からの補正処理
            start = hit.position;
            end = correction;

            // 補正後の位置を設定
            position = start;
        }
        else
        {
            position.x += totalVelocity.x * elapsedTime;
            position.z += totalVelocity.z * elapsedTime;
        }
    }

    GetGameObject()->transform_->SetWorldPosition(position);
}

// 力を追加 (非最大速度用)
void MovementCom::AddNonMaxSpeedForce(const DirectX::XMFLOAT3& force)
{
    nonMaxSpeedVelocity_.x += force.x;
    nonMaxSpeedVelocity_.z += force.z;
}

// 力を追加
void MovementCom::AddForce(const DirectX::XMFLOAT3& force)
{
    velocity_.x += force.x;
    velocity_.y += force.y;
    velocity_.z += force.z;
}