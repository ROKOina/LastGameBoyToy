#include "Logger.h"
#include "Misc.h"
#include "Model.h"
#include "Graphics/Graphics.h"

//コンストラクタ
Model::Model(std::shared_ptr<ModelResource> resource)
{
    this->resource = resource;

    // ノード
    const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

    nodes.resize(resNodes.size());
    for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
    {
        auto&& src = resNodes.at(nodeIndex);
        auto&& dst = nodes.at(nodeIndex);

        dst.nodeIndex = nodeIndex;
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

//ノード検索　上半身分別
Model::Node* Model::FindNode(const char* name)
{
    //全てのノードを総当たりで名前比較する
    for (Model::Node& node : nodes)
    {
        if (strcmp(node.name, name) == 0)
        {
            return &node;
        }
    }

    //見つからなかった
    return nullptr;
}

//特定のノードの番号検索
int Model::FindNodeIndex(const char* name)
{
    int nodeCount = static_cast<int>(nodes.size());
    for (int i = 0; i < nodeCount; ++i)
    {
        if (strcmp(nodes[i].name, name) == 0)
        {
            return nodes[i].nodeIndex;
        }
    }

    return -1;
}