#include "MovementCom.h"
#include <imgui.h>
#include "Math/Mathf.h"
#include "Component/System/TransformCom.h"
#include "Component/System/RayCastManager.h"
#include "Graphics/Graphics.h"
#include "Phsix\Physxlib.h"
#include <random>
#include "SystemStruct\TimeManager.h"

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

    //最後に最大スピード初期化
    moveMaxSpeed_ = firstMoveMaxSpeed;
}

// GUI描画
void MovementCom::OnGUI()
{
    ImGui::DragFloat3((char*)u8"速力", &velocity_.x, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat3((char*)u8"最大加速度", &nonMaxSpeedVelocity_.x);
    ImGui::DragFloat((char*)u8"重力", &gravity_, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat((char*)u8"重力影響度", &gravityeffect, 0.01f, 0.0f, 30.0f);
    ImGui::DragFloat((char*)u8"落下スピード", &fallspeed, 0.1f, -100.0f, 0.0f);
    ImGui::DragFloat((char*)u8"摩擦", &friction_, 0.01f, 0.0f, 40.0f);
    ImGui::DragFloat((char*)u8"最大速度", &moveMaxSpeed_, 0.1f, 0.0f, 30.0f);
    ImGui::DragFloat((char*)u8"加速度", &moveAcceleration_, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat((char*)u8"上昇速度", &risespeed, 0.01f, 0.0f, 30.0f);
    ImGui::DragFloat((char*)u8"最大上昇速度", &maxrisespeed, 0.01f, 0.0f, 70.0f);
    ImGui::Checkbox((char*)u8"接地", &onGround_);
    ImGui::Checkbox((char*)u8"レイキャスト", &isRaycast);
}

// 縦方向移動更新
void MovementCom::VerticalUpdate(float elapsedTime)
{
    // 重力による速力の変化
    velocity_.y -= gravity_ * gravityeffect * elapsedTime;
    velocity_.y = (std::max)(velocity_.y, fallspeed);

    // 最大速度の無い摩擦計算
    // 摩擦力
    float friction = friction_ * (elapsedTime * Graphics::Instance().GetWorldSpeed() * GetGameObject()->GetObjSpeed());

    nonMaxSpeedVelocity_.y -= friction;
    if (nonMaxSpeedVelocity_.y * nonMaxSpeedVelocity_.y < 0.1f)
        nonMaxSpeedVelocity_.y = 0;
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
        end.y += (moveVec * elapsedTime) - 0.1f;

        // 判定
        RayCastManager::Result hit;
        PxRaycastBuffer buffer;
        if (isRaycast && PhysXLib::Instance().RayCast_PhysX(start, Mathf::Normalize(end - start), Mathf::Length(end - start), buffer, PhysXLib::CollisionLayer::Stage))
        {
            // 地面に接地している
            position.y = buffer.block.position.y;
            velocity_.y = 0;
            nonMaxSpeedVelocity_.y = 0;
            onGround_ = true;
        }
        else
        {
            position.y += moveVec * elapsedTime;
            onGround_ = false;
        }
    }

    // 着地した瞬間を判定
    justLanded_ = !wasOnGround_ && onGround_;

    // 現在の状態を次のフレーム用に記録
    wasOnGround_ = onGround_;

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
        DirectX::XMFLOAT3 end = {
            position.x + moveVec.x * elapsedTime,
            position.y + stepOffset,
            position.z + moveVec.z * elapsedTime
        };

        //前回の結果を保持
        wasOnGround_ = onWall_;

        // SphereCastによる壁判定
        PxRaycastBuffer buffer;

        static bool wasColliding = false;  // 前フレームで壁に衝突していたか
        if (isRaycast && PhysXLib::Instance().RayCast_PhysX(
            start, Mathf::Normalize(end - start), Mathf::Length(end - start) + advanceOffset, buffer, PhysXLib::CollisionLayer::Stage))
        {
            DirectX::XMFLOAT3 p = {};
            p.x = buffer.block.position.x;
            p.y = buffer.block.position.y;
            p.z = buffer.block.position.z;

            DirectX::XMFLOAT3 n = {};
            n = Mathf::Normalize(start - end);
            p = p + (n * advanceOffset);

            position.x = p.x;
            position.z = p.z;

            if (useWallSride_)
            {
                // 壁に衝突した場合、壁に沿ったスライドベクトルを計算
                DirectX::XMVECTOR Start = XMLoadFloat3(&start);
                DirectX::XMVECTOR End = XMLoadFloat3(&end);
                DirectX::XMVECTOR MoveVec = DirectX::XMVectorSubtract(End, Start);

                // 壁の法線を取得
                DirectX::XMFLOAT3 normal = { buffer.block.normal.x, buffer.block.normal.y, buffer.block.normal.z };
                DirectX::XMVECTOR Normal = XMLoadFloat3(&normal);

                // 壁に沿ったスライドベクトルの計算
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(MoveVec, Normal);
                DirectX::XMVECTOR SlideVec = DirectX::XMVectorSubtract(MoveVec, DirectX::XMVectorMultiply(Normal, Dot));

                // スライドベクトルと補正ベクトルを加算して補正位置を計算
                DirectX::XMFLOAT3 correctedPosition;
                DirectX::XMStoreFloat3(&correctedPosition, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&p), SlideVec));

                position.x = correctedPosition.x;
                position.z = correctedPosition.z;
            }
            onWall_ = true;
        }
        else
        {
            // 壁に衝突しない場合、通常の移動
            position.x += moveVec.x * elapsedTime;
            position.z += moveVec.z * elapsedTime;
            wasColliding = false;  // 壁に衝突していないので、補正をリセット

            onWall_ = false;
        }
    }

    // 着地した瞬間を判定
    justHitWall_ = !wasOnWall_ && onWall_;

    GetGameObject()->transform_->SetWorldPosition(position);
}

// 力を追加 (非最大速度用)
void MovementCom::AddNonMaxSpeedForce(const DirectX::XMFLOAT3& force)
{
    nonMaxSpeedVelocity_.x += force.x;
    nonMaxSpeedVelocity_.z += force.z;
}

//上昇関数
void MovementCom::Rising(float elapsedTime)
{
    // 経過フレーム
    float elapsedFrame = 60.0f * elapsedTime;
    velocity_.y += risespeed * elapsedFrame;
    velocity_.y = (std::min)(velocity_.y, maxrisespeed);
}

// 力を追加
void MovementCom::AddForce(const DirectX::XMFLOAT3& force)
{
    velocity_.x += force.x * moveAcceleration_;
    velocity_.y += force.y * moveAcceleration_;
    velocity_.z += force.z * moveAcceleration_;
}

//ランダム方向に飛ばす
void MovementCom::ApplyRandomForce(float forcestrength, float yforce)
{
    // ランダムな X, Z 成分を生成
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    // X, Z でランダム方向ベクトルを作成
    DirectX::XMVECTOR randomVec = DirectX::XMVectorSet(dist(gen), dist(gen), dist(gen), 0.0f);

    // ゼロベクトルかどうかを確認
    if (DirectX::XMVector3Equal(randomVec, DirectX::XMVectorZero()))
    {
        // ゼロベクトルの場合はデフォルトの方向を設定（例えばZ軸方向）
        randomVec = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    }

    // ベクトルを正規化
    randomVec = DirectX::XMVector3Normalize(randomVec);

    // X, Z 成分に力の大きさを掛ける
    randomVec = DirectX::XMVectorScale(randomVec, forcestrength);

    // Y成分を追加
    DirectX::XMFLOAT3 force;
    DirectX::XMStoreFloat3(&force, randomVec);
    force.y = yforce; // Y成分は直接設定

    // AddForce を呼び出し
    velocity_.y += force.y;
    AddNonMaxSpeedForce(force);
}