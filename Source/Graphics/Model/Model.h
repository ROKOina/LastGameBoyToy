#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "ModelResource.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <unordered_map>

class Model
{
public:
    Model(std::shared_ptr<ModelResource> resource);
    ~Model() {}

    struct Node
    {
        int nodeIndex;
        const char* name;
        Node* parent;
        DirectX::XMFLOAT3	scale;
        DirectX::XMFLOAT4	rotate;
        DirectX::XMFLOAT3	translate;
        DirectX::XMFLOAT4X4	localTransform;
        DirectX::XMFLOAT4X4	worldTransform;
        std::vector<int>    layer;
        std::vector<Node*>	children;
    };

    //コリジョンのパラメータ
    struct CollsionParameter
    {
        int nodeid = {};
        int endnodeid = {};
        int collsiontype = {};
        int parttype = {};
        float radius = 0.0f;
        float height = 0.0f;
        DirectX::XMFLOAT3 scale = { 0,0,0 };
        DirectX::XMFLOAT3 offsetpos = { 0,0,0 };

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    std::unordered_map<int, std::vector<Model::CollsionParameter>>cp;

    // 行列計算
    void UpdateTransform(const DirectX::XMMATRIX& Transform);

    const std::vector<Node>& GetNodes() const { return nodes; }
    std::vector<Node>& GetNodes() { return nodes; }
    const ModelResource* GetResource() const { return resource.get(); }
    ModelResource* GetResource() { return resource.get(); }

#ifdef _DEBUG

#endif // _DEBUG

    //シリアライズに適用させる為の処理
    void CopyModel();

    //オープンノードをした時に逆に情報を入れる
    void CopyRefrectModel();

public:
    //ノード検索　上半身分別
    Node* FindNode(const char* name);

    //特定のノードの番号検索
    int FindNodeIndex(const char* name);

    //ノードのimgui
    void ImGui(Model::Node* node);

    //シリアライズ
    void Serialize();

    //デシリアライズ
    void Deserialize(const char* filename);

    //読み込み
    void LoadDesirialize();   

    // マテリアル情報のみ読み込む
    void LoadMaterial(ID3D11Device* device, const char* filename);

    std::shared_ptr<std::vector<ModelResource::Material>> GetAssignMaterials() { return assignMaterials; }

    //imguiguizmo
    void BoneGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    //子ノード検索
    void GetAllDescendants(int parentNodeId, std::vector<Model::Node*>& descendants);

    //node
    Model::Node* selectionNode = nullptr;

private:

    enum Layer
    {
        ALLBODY,
        UPPER,
        LOWER,
        LAYERMAX
    };

private:
    std::shared_ptr<ModelResource>	resource;
    std::shared_ptr<std::vector<ModelResource::Material>> assignMaterials;
    
    std::vector<Node> nodes;
    ImGuizmo::OPERATION	guizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE guizmoMode = ImGuizmo::LOCAL;
};