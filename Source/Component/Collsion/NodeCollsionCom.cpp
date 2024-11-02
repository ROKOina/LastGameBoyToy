#include "NodeCollsionCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Component/Renderer/InstanceRendererCom.h"
#include <Math\Mathf.h>

//コンストラクタ
NodeCollsionCom::NodeCollsionCom(const char* filename)
{
    m_filename = filename;
}

//開始処理
void NodeCollsionCom::Start()
{
    // モデルからリソースを取得
    GetGameObject()->GetComponent<RendererCom>() != nullptr ? model = GetGameObject()->GetComponent<RendererCom>()->GetModel() : model = GetGameObject()->GetComponent<InstanceRenderer>()->GetModel();

    if (m_filename != nullptr && model != nullptr)
    {
        model->Deserialize(m_filename);
    }
}

//更新処理
void NodeCollsionCom::Update(float elapsedTime)
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

//GUI描画
void NodeCollsionCom::OnGUI()
{
    // 保存および読み込みボタン
    if (ImGui::Button("Save"))
    {
        model->Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        model->LoadDesirialize();
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
}