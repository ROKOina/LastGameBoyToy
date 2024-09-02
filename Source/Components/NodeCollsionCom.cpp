#include "NodeCollsionCom.h"
#include "RendererCom.h"
#include <SystemStruct/TransformUtils.h>

//コンストラクタ
NodeCollsionCom::NodeCollsionCom(const char* filename)
{
    m_filename = filename;
}

//開始処理
void NodeCollsionCom::Start()
{
    // モデルからリソースを取得
    model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    if (m_filename != nullptr && model != nullptr)
    {
        model->Deserialize(m_filename);
    }
}

//更新処理
void NodeCollsionCom::Update(float elapsedTime)
{
    // 衝突パラメータの高さを自動計算
    for (auto& cp : model->cp)
    {
        DirectX::XMFLOAT3 pos =
        {
            model->GetNodes()[cp.nodeid].worldTransform._41,
            model->GetNodes()[cp.nodeid].worldTransform._42,
            model->GetNodes()[cp.nodeid].worldTransform._43
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
            // 常にデバッグプリミティブを描画
            switch (static_cast<CollsionType>(cp.collsiontype))
            {
            case NodeCollsionCom::CollsionType::SPHER:
                // debugprimitive描画
                Graphics::Instance().GetDebugRenderer()->DrawSphere(pos, cp.radius, { 1, 0, 0, 1 });
                break;

            case NodeCollsionCom::CollsionType::CYLINDER:
                // debugprimitive描画
                Graphics::Instance().GetDebugRenderer()->DrawCylinder(pos, endpos, cp.radius, cp.height, { 1, 0, 0, 1 });
                break;

            default:
                break;
            }
        }
    }
}

//GUI描画
void NodeCollsionCom::OnGUI()
{
    static std::unordered_map<int, bool> nodeCreationState;

    if (model != nullptr)
    {
        // モデルノードをImGuiで表示
        for (Model::Node& node : model->GetNodes())
        {
            if (node.parent == nullptr)
            {
                model->ImGui(&node);
            }
        }

        // 常にデバッグプリミティブを描画するために、全ての衝突パラメータをチェック
        for (const auto& cp : model->cp)
        {
            DirectX::XMFLOAT3 pos =
            {
                model->GetNodes()[cp.nodeid].worldTransform._41,
                model->GetNodes()[cp.nodeid].worldTransform._42,
                model->GetNodes()[cp.nodeid].worldTransform._43
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
        }

        ImGui::Checkbox((char*)u8"デバッグ描画", &debugrender);

        // 衝突パラメータを表示
        for (size_t i = 0; i < model->cp.size(); ++i)
        {
            auto& cp = model->cp[i];

            if (model->selectionNode != nullptr && cp.nodeid == model->selectionNode->nodeIndex)
            {
                ImGui::PushID(static_cast<int>(i));

                // コリジョンの形状で出すimguiを決める
                const char* collisionTypeNames[] = { "Sphere", "Cylinder" };
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

                case NodeCollsionCom::CollsionType::CYLINDER:

                    // ノード選択用のリストを表示
                    if (model->selectionNode != nullptr)
                    {
                        if (ImGui::BeginCombo("EndNode Selection", "Select a Node"))
                        {
                            for (const auto& node : model->GetNodes())
                            {
                                bool isSelected = (node.nodeIndex == model->selectionNode->nodeIndex);
                                if (ImGui::Selectable(node.name, isSelected))
                                {
                                    // ノード名を選択してendnodeidに設定
                                    for (auto& cp : model->cp)
                                    {
                                        if (cp.nodeid == model->selectionNode->nodeIndex)
                                        {
                                            cp.endnodeid = node.nodeIndex;
                                            break;
                                        }
                                    }
                                }
                                if (isSelected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }

                    // ノード選択のためのドロップダウンメニュー
                    if (ImGui::BeginCombo("EndNodeChilderen", cp.endnodeid >= 0 ? model->GetNodes()[cp.endnodeid].name : "None"))
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
                    // 衝突パラメータの設定
                    ImGui::DragFloat("Radius", &cp.radius, 0.1f, 0.0f, 5.0f);
                    break;

                default:
                    break;
                }

                // 削除ボタン
                if (ImGui::Button("Delete"))
                {
                    model->cp.erase(model->cp.begin() + i);
                    // ノードが削除されたので、再度Create可能にする
                    nodeCreationState[cp.nodeid] = false;
                    ImGui::PopID();
                    break; // 削除したらループを終了
                }

                ImGui::PopID();
            }
        }

        // 新しい衝突パラメータを作成
        if (model->selectionNode != nullptr)
        {
            int selectedNodeId = model->selectionNode->nodeIndex;
            bool hasCollisionParameter = false;

            // 既に選択されたノードに衝突パラメータが存在するか確認
            for (const auto& cp : model->cp)
            {
                if (cp.nodeid == selectedNodeId)
                {
                    hasCollisionParameter = true;
                    break;
                }
            }

            if (!hasCollisionParameter)
            {
                if (ImGui::Button("Create"))
                {
                    Model::CollsionParameter newParam;
                    newParam.nodeid = selectedNodeId;
                    newParam.endnodeid = -1; // 初期状態では無効な値を設定
                    newParam.collsiontype = static_cast<int>(CollsionType::SPHER); // 初期値をSPHERに設定
                    model->cp.emplace_back(newParam);
                    nodeCreationState[selectedNodeId] = true; // 作成済みとして記録
                }
            }
            else
            {
                ImGui::Text((char*)u8"このノードには既に衝突パラメータが作成されています");
            }
        }

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
    }
}