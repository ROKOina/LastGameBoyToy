#include "NodeCollsionCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Component/Renderer/InstanceRendererCom.h"
#include <Math\Mathf.h>
#include "Component\System\TransformCom.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <SystemStruct\Dialog.h>
#include <SystemStruct\Logger.h>
#include <fstream>

CEREAL_CLASS_VERSION(NodeCollsionCom::StaticCollsionParameter, 1)

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
void NodeCollsionCom::StaticCollsionParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(collsiontype),
        CEREAL_NVP(radius),
        CEREAL_NVP(scale),
        CEREAL_NVP(offsetpos)
    );
}

//コンストラクタ
NodeCollsionCom::NodeCollsionCom(const char* filename)
{
    m_filename = filename;
}

//開始処理
void NodeCollsionCom::Start()
{
    // モデルが存在する場合にのみ取得する
    if (const auto& renderer = GetGameObject()->GetComponent<RendererCom>())
    {
        model = renderer->GetModel();
    }
    else if (const auto& instanceRenderer = GetGameObject()->GetComponent<InstanceRenderer>())
    {
        model = instanceRenderer->GetModel();
    }

    // モデルが存在し、かつファイル名が設定されている場合のみデシリアライズ
    if (m_filename != nullptr && model != nullptr)
    {
        model->Deserialize(m_filename);
    }
    else if (m_filename != nullptr)
    {
        Deserialize(m_filename);
    }
}

//更新処理
void NodeCollsionCom::Update(float elapsedTime)
{
    // モデルがない場合はゲームオブジェクトの位置に基づいたシンプルなコリジョン
    if (model == nullptr)
    {
        // 全てのノードに対して、コリジョンパラメータを処理
        for (auto& nodeEntry : scp)
        {
            for (auto& cp : nodeEntry.second)
            {
                DirectX::XMFLOAT3 pos =
                {
                    GetGameObject()->transform_->GetWorldPosition().x + cp.offsetpos.x,
                    GetGameObject()->transform_->GetWorldPosition().y + cp.offsetpos.y,
                    GetGameObject()->transform_->GetWorldPosition().z + cp.offsetpos.z
                };

                // `pos` と `endpos` までの距離を計算
                DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&pos);

                //デバッグプリミティブ描画
                if (debugrender)
                {
                    switch (static_cast<CollsionType>(cp.collsiontype))
                    {
                    case NodeCollsionCom::CollsionType::SPHER:
                        Graphics::Instance().GetDebugRenderer()->DrawSphere(pos, cp.radius, { 1, 0, 0, 1 });
                        break;

                    case NodeCollsionCom::CollsionType::BOX:
                        DirectX::XMFLOAT4X4 mat = GetGameObject()->transform_->GetWorldTransform();
                        DirectX::XMFLOAT3 m = { mat._11,mat._12,mat._13 };
                        m = Mathf::Normalize(m);
                        mat._11 = m.x;
                        mat._12 = m.y;
                        mat._13 = m.z;
                        m = { mat._21,mat._22,mat._23 };
                        m = Mathf::Normalize(m);
                        mat._21 = m.x;
                        mat._22 = m.y;
                        mat._23 = m.z;
                        m = { mat._31,mat._32,mat._33 };
                        m = Mathf::Normalize(m);
                        mat._31 = m.x;
                        mat._32 = m.y;
                        mat._33 = m.z;
                        DirectX::XMMATRIX worldMat = DirectX::XMLoadFloat4x4(&mat);
                        DirectX::XMFLOAT4 ro;
                        DirectX::XMStoreFloat4(&ro, DirectX::XMQuaternionRotationMatrix(worldMat));

                        Graphics::Instance().GetDebugRenderer()->DrawBox(pos, cp.scale, { 1, 0, 0, 1 }, ro);
                        break;

                    default:
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // 全てのノードに対して、コリジョンパラメータを処理
        for (auto& nodeEntry : model->cp)
        {
            for (auto& cp : nodeEntry.second)
            {
                DirectX::XMFLOAT3 pos =
                {
                    model->GetNodes()[cp.nodeid].worldTransform._41 + cp.offsetpos.x,
                    model->GetNodes()[cp.nodeid].worldTransform._42 + cp.offsetpos.y,
                    model->GetNodes()[cp.nodeid].worldTransform._43 + cp.offsetpos.z
                };

                DirectX::XMFLOAT3 endpos = { 0.0f, 0.0f, 0.0f };
                if (cp.endnodeid >= 0 && cp.endnodeid < model->GetNodes().size())
                {
                    endpos =
                    {
                        model->GetNodes()[cp.endnodeid].worldTransform._41,
                        model->GetNodes()[cp.endnodeid].worldTransform._42,
                        model->GetNodes()[cp.endnodeid].worldTransform._43
                    };
                }

                // `pos` と `endpos` までの距離を計算
                DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&pos);
                DirectX::XMVECTOR endposVec = DirectX::XMLoadFloat3(&endpos);
                DirectX::XMVECTOR distanceVec = DirectX::XMVectorSubtract(endposVec, posVec);
                float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(distanceVec));

                // 高さを設定
                cp.height = distance;

                //デバッグプリミティブ描画
                if (debugrender)
                {
                    switch (static_cast<CollsionType>(cp.collsiontype))
                    {
                    case NodeCollsionCom::CollsionType::SPHER:
                        Graphics::Instance().GetDebugRenderer()->DrawSphere(pos, cp.radius, { 1, 0, 0, 1 });
                        break;

                    case NodeCollsionCom::CollsionType::CYLINDER:
                        Graphics::Instance().GetDebugRenderer()->DrawCylinder(pos, endpos, cp.radius, cp.height, { 1, 0, 0, 1 });
                        break;

                    case NodeCollsionCom::CollsionType::BOX:
                        DirectX::XMFLOAT4X4 mat = model->GetNodes()[cp.nodeid].worldTransform;
                        DirectX::XMFLOAT3 m = { mat._11,mat._12,mat._13 };
                        m = Mathf::Normalize(m);
                        mat._11 = m.x;
                        mat._12 = m.y;
                        mat._13 = m.z;
                        m = { mat._21,mat._22,mat._23 };
                        m = Mathf::Normalize(m);
                        mat._21 = m.x;
                        mat._22 = m.y;
                        mat._23 = m.z;
                        m = { mat._31,mat._32,mat._33 };
                        m = Mathf::Normalize(m);
                        mat._31 = m.x;
                        mat._32 = m.y;
                        mat._33 = m.z;
                        DirectX::XMMATRIX worldMat = DirectX::XMLoadFloat4x4(&mat);
                        DirectX::XMFLOAT4 ro;
                        DirectX::XMStoreFloat4(&ro, DirectX::XMQuaternionRotationMatrix(worldMat));

                        Graphics::Instance().GetDebugRenderer()->DrawBox(pos, cp.scale, { 1, 0, 0, 1 }, ro);
                        break;

                    default:
                        break;
                    }
                }
            }
        }
    }
}

//GUI描画
void NodeCollsionCom::OnGUI()
{
    // 保存および読み込みボタン
    if (ImGui::Button("Save"))
    {
        if (model == nullptr)
        {
            Serialize();
        }
        else
        {
            model->Serialize();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        if (model == nullptr)
        {
            LoadDeserialize();
        }
        else
        {
            model->LoadDesirialize();
        }
    }

    static std::unordered_map<int, bool> nodeCreationState;

    if (model != nullptr)
    {
        for (Model::Node& node : model->GetNodes())
        {
            if (node.parent == nullptr)
            {
                model->ImGui(&node);
            }
        }

        ImGui::Checkbox((char*)u8"デバッグ描画", &debugrender);

        for (auto& nodeEntry : model->cp)
        {
            for (size_t i = 0; i < nodeEntry.second.size(); ++i)
            {
                auto& cp = nodeEntry.second[i];

                if (model->selectionNode != nullptr && cp.nodeid == model->selectionNode->nodeIndex)
                {
                    ImGui::PushID(static_cast<int>(i));

                    const char* collisionTypeNames[] = { "Sphere", "Cylinder", "Box" };
                    int collisionTypeIndex = static_cast<int>(cp.collsiontype);
                    if (ImGui::Combo("Collision Type", &collisionTypeIndex, collisionTypeNames, IM_ARRAYSIZE(collisionTypeNames)))
                    {
                        cp.collsiontype = static_cast<int>(collisionTypeIndex);
                    }

                    const char* bodytypename[] = { "NONE", "BODY", "FACE" ,"RIGHTHAND","LEFTHAND","RIGHTLEG","LEFTLEG" };
                    int bodyTypeIndex = static_cast<int>(cp.parttype);
                    if (ImGui::Combo("Body Type", &bodyTypeIndex, bodytypename, IM_ARRAYSIZE(bodytypename)))
                    {
                        cp.parttype = static_cast<int>(bodyTypeIndex);
                    }

                    switch (static_cast<CollsionType>(cp.collsiontype))
                    {
                    case NodeCollsionCom::CollsionType::SPHER:
                        ImGui::DragFloat("Radius", &cp.radius, 0.1f, 0.0f, 5.0f);
                        break;

                    case NodeCollsionCom::CollsionType::CYLINDER:
                        if (model->selectionNode != nullptr)
                        {
                            // "EndNode Selection" ドロップダウンリストの選択処理
                            if (ImGui::BeginCombo("EndNode Selection", cp.endnodeid >= 0 ? model->GetNodes()[cp.endnodeid].name : "Select a Node"))
                            {
                                for (const auto& node : model->GetNodes())
                                {
                                    bool isSelected = (node.nodeIndex == cp.endnodeid);
                                    if (ImGui::Selectable(node.name, isSelected))
                                    {
                                        cp.endnodeid = node.nodeIndex;
                                    }
                                    if (isSelected)
                                    {
                                        ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            }
                        }

                        // "EndNodeChildren" ドロップダウンリストの選択処理
                        if (ImGui::BeginCombo("EndNodeChildren", cp.endnodeid >= 0 ? model->GetNodes()[cp.endnodeid].name : "None"))
                        {
                            std::vector<Model::Node*> descendants;
                            model->GetAllDescendants(model->selectionNode->nodeIndex, descendants);

                            for (auto& node : descendants)
                            {
                                bool isSelected = (cp.endnodeid == node->nodeIndex);
                                if (ImGui::Selectable(node->name, isSelected))
                                {
                                    cp.endnodeid = node->nodeIndex;
                                }
                                if (isSelected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::DragFloat("Radius", &cp.radius, 0.1f, 0.0f, 5.0f);
                        break;

                    case NodeCollsionCom::CollsionType::BOX:
                        ImGui::DragFloat3("Scale", &cp.scale.x, 0.1f, 0.0f, 5.0f);
                        ImGui::DragFloat3("OffsetPos", &cp.offsetpos.x);
                        break;

                    default:
                        break;
                    }

                    if (ImGui::Button("Delete"))
                    {
                        nodeEntry.second.erase(nodeEntry.second.begin() + i);
                        ImGui::PopID();
                        break;
                    }

                    ImGui::PopID();
                }
            }
        }

        if (model->selectionNode != nullptr)
        {
            int selectedNodeId = model->selectionNode->nodeIndex;

            if (ImGui::Button("Create Collision Parameter"))
            {
                // 新しいコリジョンパラメータを追加する際に、選択されたノードの情報を正しく設定
                model->cp[selectedNodeId].emplace_back(Model::CollsionParameter{ selectedNodeId });
            }
        }
    }
    else
    {
        ImGui::Checkbox((char*)u8"デバッグ描画", &debugrender);

        for (auto& nodeEntry : scp)
        {
            for (size_t i = 0; i < nodeEntry.second.size(); ++i)
            {
                auto& cp = nodeEntry.second[i];

                ImGui::PushID(static_cast<int>(i));

                const char* collisionTypeNames[] = { "Sphere", "Cyllinder","Box" };
                int collisionTypeIndex = static_cast<int>(cp.collsiontype);
                if (ImGui::Combo("Collision Type", &collisionTypeIndex, collisionTypeNames, IM_ARRAYSIZE(collisionTypeNames)))
                {
                    cp.collsiontype = static_cast<int>(collisionTypeIndex);
                }

                switch (static_cast<CollsionType>(cp.collsiontype))
                {
                case NodeCollsionCom::CollsionType::SPHER:
                    ImGui::DragFloat("Radius", &cp.radius, 0.1f, 0.0f, 5.0f);
                    break;

                case NodeCollsionCom::CollsionType::BOX:
                    ImGui::DragFloat3("Scale", &cp.scale.x, 0.1f, 0.0f, 5.0f);
                    ImGui::DragFloat3("OffsetPos", &cp.offsetpos.x);
                    break;

                default:
                    break;
                }

                if (ImGui::Button("Delete"))
                {
                    nodeEntry.second.erase(nodeEntry.second.begin() + i);
                    ImGui::PopID();
                    break;
                }

                ImGui::PopID();
            }
        }

        if (ImGui::Button("Create Collision Parameter"))
        {
            int selectedNodeId = 0;

            // 新しいコリジョンパラメータを追加する際に、選択されたノードの情報を正しく設定
            scp[selectedNodeId].emplace_back(StaticCollsionParameter{ selectedNodeId });
        }
    }
}

//シリアリズ
void NodeCollsionCom::Serialize()
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
                    CEREAL_NVP(scp)
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

// デシリアライズ
void NodeCollsionCom::Deserialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(scp)
            );
        }
        catch (...)
        {
            LOG("nodecollsion deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

// デシリアライズの読み込み
void NodeCollsionCom::LoadDeserialize()
{
    static const char* filter = "nodecollsion Files(*.nodecollsion)\0*.nodecollsion;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Deserialize(filename);
    }
}