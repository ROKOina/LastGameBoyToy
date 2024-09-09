#include "Logger.h"
#include "Misc.h"
#include "Model.h"
#include "Graphics/Graphics.h"
#include "Dialog.h"
#include <fstream>
#include <imgui.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <SystemStruct/TransformUtils.h>

CEREAL_CLASS_VERSION(Model::CollsionParameter, 1)

// シリアライズ
namespace DirectX
{
    template<class Archive>
    void serialize(Archive& archive, XMUINT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

template<class Archive>
void Model::CollsionParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(nodeid),
        CEREAL_NVP(endnodeid),
        CEREAL_NVP(collsiontype),
        CEREAL_NVP(radius),
        CEREAL_NVP(height),
        CEREAL_NVP(scale),
        CEREAL_NVP(offsetpos)
    );
}

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

//imgui
void Model::ImGui(Model::Node* node)
{
    // 矢印をクリック、またはノードをダブルクリックで階層を開く
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // 選択フラグ
    if (selectionNode == node)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    // 子がいない場合は矢印をつけない
    size_t child_count = node->children.size();
    if (child_count <= 0)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // ノードに関連付けられたCollisionParameterが存在するかチェック
    bool hasCollisionParam = std::any_of(cp.begin(), cp.end(), [node](const CollsionParameter& param) {
        return param.nodeid == node->nodeIndex;
        });

    // ノード名の色を設定
    if (hasCollisionParam)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // 赤色
    }

    // ツリーノードを表示
    bool opened = ImGui::TreeNodeEx(node, nodeFlags, node->name);

    // アクティブ化されたノードは選択する
    if (ImGui::IsItemClicked())
    {
        selectionNode = node;
    }

    // 色を元に戻す
    if (hasCollisionParam)
    {
        ImGui::PopStyleColor();
    }

    // 開かれている場合、子階層も同じ処理を行う
    if (opened && child_count > 0)
    {
        for (Model::Node* child : node->children)
        {
            ImGui(child);
        }
        ImGui::TreePop();
    }
}

//シリアライズ
void Model::Serialize()
{
    static const char* filter = "NodeCollsion Files(*.nodecollsion)\0*.nodecollsion;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "nodecollsion", Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        std::ofstream ostream(filename, std::ios::binary);
        if (ostream.is_open())
        {
            cereal::BinaryOutputArchive archive(ostream);

            try
            {
                archive
                (
                    CEREAL_NVP(cp)
                );
            }
            catch (...)
            {
                LOG("nodecollsion deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

//シリアライズ
void Model::Deserialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(cp)
            );
        }
        catch (...)
        {
            LOG("nodecollsion deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

//デシリアライズ読み込み
void Model::LoadDesirialize()
{
    static const char* filter = "nodecollsion Files(*.nodecollsion)\0*.nodecollsion;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Deserialize(filename);
    }
}

//imguiguizmo
void Model::BoneGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    if (selectionNode != nullptr)
    {
        // 選択ノードの行列を操作する
        ImGuizmo::Manipulate(
            &view._11, &projection._11,	// ビュー＆プロジェクション行列
            guizmoOperation,			// 操作
            guizmoMode,					// 空間
            &selectionNode->worldTransform._11,	// 操作するワールド行列
            nullptr);

        // 操作したワールド行列をローカル行列に反映
        if (selectionNode->parent != nullptr)
        {
            // 親のワールド逆行列と自身のワールド行列を乗算することで自身のローカル行列を算出できる
            DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&selectionNode->parent->worldTransform);
            DirectX::XMMATRIX ParentInverseTransform = DirectX::XMMatrixInverse(nullptr, ParentWorldTransform);
            DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&selectionNode->worldTransform);
            DirectX::XMMATRIX LocalTransform = DirectX::XMMatrixMultiply(WorldTransform, ParentInverseTransform);
            DirectX::XMStoreFloat4x4(&selectionNode->localTransform, LocalTransform);
        }
    }
}

//子ノード検索
void Model::GetAllDescendants(int parentNodeId, std::vector<Model::Node*>& descendants)
{
    for (auto& node : nodes)
    {
        if (node.parent != nullptr && node.parent->nodeIndex == parentNodeId)
        {
            descendants.push_back(&node);
            GetAllDescendants(node.nodeIndex, descendants); // 再帰的に子ノードを取得
        }
    }
}