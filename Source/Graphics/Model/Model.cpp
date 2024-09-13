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

// �V���A���C�Y
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

//�R���X�g���N�^
Model::Model(std::shared_ptr<ModelResource> resource)
{
    this->resource = resource;

    // �m�[�h
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

// �ϊ��s��v�Z
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

//�V���A���C�Y�ɓK�p������ׂ̏���
void Model::CopyModel()
{
    this->resource = resource;

    // �m�[�h
    std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

    //�R�s�[����
    nodes.resize(resNodes.size());
    for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
    {
        resNodes.at(nodeIndex).translate = nodes.at(nodeIndex).translate;
        resNodes.at(nodeIndex).scale = nodes.at(nodeIndex).scale;
        resNodes.at(nodeIndex).rotate = nodes.at(nodeIndex).rotate;
        resNodes.at(nodeIndex).layer = nodes.at(nodeIndex).layer;
    }
}

//�I�[�v���m�[�h���������ɋt�ɏ�������
void Model::CopyRefrectModel()
{
    this->resource = resource;

    // �m�[�h
    std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

    //�R�s�[����
    nodes.resize(resNodes.size());
    for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
    {
        nodes.at(nodeIndex).translate = resNodes.at(nodeIndex).translate;
        nodes.at(nodeIndex).scale = resNodes.at(nodeIndex).scale;
        nodes.at(nodeIndex).rotate = resNodes.at(nodeIndex).rotate;
        nodes.at(nodeIndex).layer = resNodes.at(nodeIndex).layer;
    }
}

//�m�[�h�����@�㔼�g����
Model::Node* Model::FindNode(const char* name)
{
    //�S�Ẵm�[�h�𑍓�����Ŗ��O��r����
    for (Model::Node& node : nodes)
    {
        if (strcmp(node.name, name) == 0)
        {
            return &node;
        }
    }

    //������Ȃ�����
    return nullptr;
}

//����̃m�[�h�̔ԍ�����
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
    // �����N���b�N�A�܂��̓m�[�h���_�u���N���b�N�ŊK�w���J��
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // �I���t���O
    if (selectionNode == node)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    // �q�����Ȃ��ꍇ�͖������Ȃ�
    size_t child_count = node->children.size();
    if (child_count <= 0)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // �m�[�h�Ɋ֘A�t����ꂽCollisionParameter�����݂��邩�`�F�b�N
    bool hasCollisionParam = std::any_of(cp.begin(), cp.end(), [node](const CollsionParameter& param) {
        return param.nodeid == node->nodeIndex;
        });

    // �m�[�h���̐F��ݒ�
    if (hasCollisionParam)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // �ԐF
    }

    // �c���[�m�[�h��\��
    bool opened = ImGui::TreeNodeEx(node, nodeFlags, node->name);

    // �A�N�e�B�u�����ꂽ�m�[�h�͑I������
    if (ImGui::IsItemClicked())
    {
        selectionNode = node;
    }

    // �F�����ɖ߂�
    if (hasCollisionParam)
    {
        ImGui::PopStyleColor();
    }

    // �J����Ă���ꍇ�A�q�K�w�������������s��
    if (opened && child_count > 0)
    {
        for (Model::Node* child : node->children)
        {
            ImGui(child);
        }
        ImGui::TreePop();
    }
}

//�V���A���C�Y
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

//�V���A���C�Y
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

//�f�V���A���C�Y�ǂݍ���
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
        // �I���m�[�h�̍s��𑀍삷��
        ImGuizmo::Manipulate(
            &view._11, &projection._11,	// �r���[���v���W�F�N�V�����s��
            guizmoOperation,			// ����
            guizmoMode,					// ���
            &selectionNode->worldTransform._11,	// ���삷�郏�[���h�s��
            nullptr);

        // ���삵�����[���h�s������[�J���s��ɔ��f
        if (selectionNode->parent != nullptr)
        {
            // �e�̃��[���h�t�s��Ǝ��g�̃��[���h�s�����Z���邱�ƂŎ��g�̃��[�J���s����Z�o�ł���
            DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&selectionNode->parent->worldTransform);
            DirectX::XMMATRIX ParentInverseTransform = DirectX::XMMatrixInverse(nullptr, ParentWorldTransform);
            DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&selectionNode->worldTransform);
            DirectX::XMMATRIX LocalTransform = DirectX::XMMatrixMultiply(WorldTransform, ParentInverseTransform);
            DirectX::XMStoreFloat4x4(&selectionNode->localTransform, LocalTransform);
        }
    }
}

//�q�m�[�h����
void Model::GetAllDescendants(int parentNodeId, std::vector<Model::Node*>& descendants)
{
    for (auto& node : nodes)
    {
        if (node.parent != nullptr && node.parent->nodeIndex == parentNodeId)
        {
            descendants.push_back(&node);
            GetAllDescendants(node.nodeIndex, descendants); // �ċA�I�Ɏq�m�[�h���擾
        }
    }
}