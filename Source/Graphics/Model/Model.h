#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "ModelResource.h"
#include <imgui.h>
#include <ImGuizmo.h>

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

    //�R���W�����̃p�����[�^
    struct CollsionParameter
    {
        int nodeid = {};
        int endnodeid = {};
        int collsiontype = {};
        float radius = 0.0f;
        float height = 0.0f;
        DirectX::XMFLOAT3 scale = { 0,0,0 };
        DirectX::XMFLOAT3 offsetpos = { 0,0,0 };

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    std::vector<CollsionParameter>cp;

    // �s��v�Z
    void UpdateTransform(const DirectX::XMMATRIX& Transform);

    const std::vector<Node>& GetNodes() const { return nodes; }
    std::vector<Node>& GetNodes() { return nodes; }
    const ModelResource* GetResource() const { return resource.get(); }
    ModelResource* GetResource() { return resource.get(); }

#ifdef _DEBUG

#endif // _DEBUG

    //�V���A���C�Y�ɓK�p������ׂ̏���
    void CopyModel();

    //�I�[�v���m�[�h���������ɋt�ɏ�������
    void CopyRefrectModel();

public:
    //�m�[�h�����@�㔼�g����
    Node* FindNode(const char* name);

    //����̃m�[�h�̔ԍ�����
    int FindNodeIndex(const char* name);

    //�m�[�h��imgui
    void ImGui(Model::Node* node);

    //�V���A���C�Y
    void Serialize();

    //�f�V���A���C�Y
    void Deserialize(const char* filename);

    //�ǂݍ���
    void LoadDesirialize();

    //imguiguizmo
    void BoneGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    //�q�m�[�h����
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
    std::vector<Node> nodes;
    ImGuizmo::OPERATION	guizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE guizmoMode = ImGuizmo::LOCAL;
};