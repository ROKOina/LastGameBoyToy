#include "Logger.h"
#include "Misc.h"
#include "Model.h"
#include "Graphics/Graphics.h"

//コンストラクタ
Model::Model(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    resource = std::shared_ptr<ModelResource>();
    resource->Load(device, filename);

    // ノード
    const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

    nodes.resize(resNodes.size());
    for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
    {
        auto&& src = resNodes.at(nodeIndex);
        auto&& dst = nodes.at(nodeIndex);

        dst.name = src.name.c_str();
        dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
        dst.scale = src.scale;
        dst.rotate = src.rotate;
        dst.translate = src.translate;
        dst.layer = src.layer;

        if (dst.parent != nullptr)
        {
            dst.parent->children.emplace_back(&dst);
        }
    }
}

// アニメーション再生
void Model::PlayAnimation(int animationIndex, bool loop)
{
    currentAnimation = animationIndex;
    loopAnimation = loop;
    endAnimation = false;
    currentSeconds = 0.0f;
}

// アニメーション停止
void Model::StopAnimation()
{
    currentAnimation = -1;
}

// アニメーション計算
void Model::UpdateAnimation(float elapsedTime)
{
    if (currentAnimation < 0)
    {
        return;
    }

    if (resource->GetAnimations().empty())
    {
        return;
    }

    const ModelResource::Animation& animation = resource->GetAnimations().at(currentAnimation);

    const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
    int keyCount = static_cast<int>(keyframes.size());
    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        // 現在の時間がどのキーフレームの間にいるか判定する
        const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
        if (currentSeconds >= keyframe0.seconds && currentSeconds <= keyframe1.seconds)
        {
            float rate = (currentSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
            assert(nodes.size() == keyframe0.nodeKeys.size());
            assert(nodes.size() == keyframe1.nodeKeys.size());
            int nodeCount = static_cast<int>(nodes.size());
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                // ２つのキーフレーム間の補完計算
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                Node& node = nodes[nodeIndex];

                //アニメーション計算
                if (node.layer <= LAYERMAX)
                {
                    ComputeAnimation(key0, key1, rate, node);
                }
            }
            break;
        }
    }

    // 最終フレーム処理
    if (endAnimation)
    {
        endAnimation = false;
        currentAnimation = -1;
        return;
    }

    // 時間経過
    currentSeconds += elapsedTime * animation.animationspeed * speed;
    if (currentSeconds >= animation.secondsLength)
    {
        if (loopAnimation)
        {
            currentSeconds -= animation.secondsLength;
        }
        else
        {
            currentSeconds = animation.secondsLength;
            endAnimation = true;
        }
    }
}

//アニメーション計算
void Model::ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Node& node)
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

// 変換行列計算
void Model::UpdateTransform(const DirectX::XMMATRIX& Transform)
{
    for (Node& node : nodes)
    {
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
        DirectX::XMMATRIX LocalTransform = S * R * T;

        DirectX::XMMATRIX ParentTransform;
        if (node.parent != nullptr)
        {
            ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
        }
        else
        {
            ParentTransform = Transform;
        }
        DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

        DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
        DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
    }
}

//シリアライズに適用させる為の処理
void Model::CopyModel()
{
    this->resource = resource;

    // ノード
    std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

    //コピー成功
    nodes.resize(resNodes.size());
    for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
    {
        resNodes.at(nodeIndex).translate = nodes.at(nodeIndex).translate;
        resNodes.at(nodeIndex).scale = nodes.at(nodeIndex).scale;
        resNodes.at(nodeIndex).rotate = nodes.at(nodeIndex).rotate;
        resNodes.at(nodeIndex).layer = nodes.at(nodeIndex).layer;
    }
}

//オープンノードをした時に逆に情報を入れる
void Model::CopyRefrectModel()
{
    this->resource = resource;

    // ノード
    std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

    //コピー成功
    nodes.resize(resNodes.size());
    for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
    {
        nodes.at(nodeIndex).translate = resNodes.at(nodeIndex).translate;
        nodes.at(nodeIndex).scale = resNodes.at(nodeIndex).scale;
        nodes.at(nodeIndex).rotate = resNodes.at(nodeIndex).rotate;
        nodes.at(nodeIndex).layer = resNodes.at(nodeIndex).layer;
    }
}