#include "AnimationCalculation.h"

//アニメーション計算
void AnimationCalculation::ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Model::Node& node)
{
    DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
    DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
    DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
    DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
    DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
    DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

    DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
    DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
    DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

    DirectX::XMStoreFloat3(&node.scale, S);
    DirectX::XMStoreFloat4(&node.rotate, R);
    DirectX::XMStoreFloat3(&node.translate, T);
}

//アニメーション切り替え時の計算
void AnimationCalculation::ComputeSwitchAnimation(const ModelResource::NodeKeyData& key1, const float blendRate, Model::Node& node)
{
    DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
    DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
    DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);
    DirectX::XMVECTOR MS1 = DirectX::XMLoadFloat3(&node.scale);
    DirectX::XMVECTOR MR1 = DirectX::XMLoadFloat4(&node.rotate);
    DirectX::XMVECTOR MT1 = DirectX::XMLoadFloat3(&node.translate);

    DirectX::XMVECTOR S = DirectX::XMVectorLerp(MS1, S1, blendRate);
    DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(MR1, R1, blendRate);
    DirectX::XMVECTOR T = DirectX::XMVectorLerp(MT1, T1, blendRate);

    DirectX::XMStoreFloat3(&node.scale, S);
    DirectX::XMStoreFloat4(&node.rotate, R);
    DirectX::XMStoreFloat3(&node.translate, T);
}

void AnimationCalculation::ComputeWalkIdleAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, float blendRate, float walkRate, Model::Node& node)
{
    DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
    DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
    DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
    DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
    DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
    DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);
    DirectX::XMVECTOR NS = DirectX::XMLoadFloat3(&node.scale);
    DirectX::XMVECTOR NR = DirectX::XMLoadFloat4(&node.rotate);
    DirectX::XMVECTOR NT = DirectX::XMLoadFloat3(&node.translate);

    DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, walkRate);
    DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, walkRate);
    DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, walkRate);

    S = DirectX::XMVectorLerp(NS, S, blendRate);
    R = DirectX::XMQuaternionSlerp(NR, R, blendRate);
    T = DirectX::XMVectorLerp(NT, T, blendRate);

    DirectX::XMStoreFloat3(&node.scale, S);
    DirectX::XMStoreFloat4(&node.rotate, R);
    DirectX::XMStoreFloat3(&node.translate, T);
}