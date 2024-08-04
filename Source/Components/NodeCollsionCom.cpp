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

        // 衝突パラメータを表示
        for (size_t i = 0; i < model->cp.size(); ++i)
        {
            auto& cp = model->cp[i];
            DirectX::XMFLOAT3 pos = {
                model->GetNodes()[cp.nodeid].worldTransform._41,
                model->GetNodes()[cp.nodeid].worldTransform._42,
                model->GetNodes()[cp.nodeid].worldTransform._43
            };
            Graphics::Instance().GetDebugRenderer()->DrawSphere(pos, cp.radius, { 1, 0, 0, 1 });

            if (model->selectionNode != nullptr && cp.nodeid == model->selectionNode->nodeIndex)
            {
                ImGui::PushID(static_cast<int>(i));
                ImGui::DragFloat("Radius", &cp.radius, 0.1f, 0.0f, 2.0f);
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
            if (nodeCreationState.find(selectedNodeId) == nodeCreationState.end() || !nodeCreationState[selectedNodeId])
            {
                if (ImGui::Button("Create"))
                {
                    Model::CollsionParameter newParam;
                    newParam.nodeid = selectedNodeId;
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