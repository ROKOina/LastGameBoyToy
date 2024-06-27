#include "FootIKCom.h"
#include "GameSource\Math\Collision.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"

void FootIKCom::Start()
{
    //足ノードを登録
    for (int i = 0; i < (int)LegNodes::Max; i++)
    {
        for (auto& node : GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetNodes())
        {
            if (node.layer == i + 3)
            {
                legNodes[i] = &node;
                rotate[i] = { &node.translate };
            }
        }
    }
    stageModel = GameObjectManager::Instance().Find("stage")->GetComponent<RendererCom>()->GetModel();
}

void FootIKCom::Update(float elassedTime)
{
    //ターゲットポジションを取得
    targetPos[(int)Legs::RIGHT] = GetTargetPosition(Legs::RIGHT);
    targetPos[(int)Legs::LEFT] = GetTargetPosition(Legs::LEFT);
}

//ターゲットポジション取得
DirectX::XMFLOAT3 FootIKCom::GetTargetPosition(Legs leg)
{
    HitResult hitresult;

    //レイの始点、終点
    DirectX::XMFLOAT3 start;
    DirectX::XMFLOAT3 end;

    //右足のターゲットポジション
    if ((int)leg == (int)Legs::RIGHT)
    {
        //膝からレイを飛ばす
        start = legNodes[(int)LegNodes::RIGHT_KNEES]->translate;
        //膝から足首のベクトル
        end = legNodes[(int)LegNodes::RIGHT_ANKLE]->translate - legNodes[(int)LegNodes::RIGHT_KNEES]->translate;
    }
    //左足のターゲットポジション
    else
    {
        //膝からレイを飛ばす
        start = legNodes[(int)LegNodes::LEFT_KNEES]->translate;
        //膝から足首のベクトル
        end = legNodes[(int)LegNodes::LEFT_ANKLE]->translate - legNodes[(int)LegNodes::LEFT_KNEES]->translate;
    }

    //正規化
    end = Mathf::Normalize(end);

    //オフセット分レイを伸ばす
    float offset = 100;
    end *= offset;

    // レイの位置をワールド空間に変換
    {
        DirectX::XMFLOAT4X4 world = GetGameObject()->transform_->GetWorldTransform();
        DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&start), DirectX::XMLoadFloat4x4(&world));
        DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&end), DirectX::XMLoadFloat4x4(&world));

        DirectX::XMStoreFloat3(&start, Start);
        DirectX::XMStoreFloat3(&end, End);
    }

    Collision::IntersectRayVsModel(
        start,
        end,
        stageModel,
        hitresult
    );

    //レイキャストの結果を返す
    return hitresult.position;
}

void FootIKCom::OnGUI()
{
    for (int i = 0; i < 6; i++)
    {
        std::string legnode = legNodes[i]->name;
        ImGui::Text(std::string("Nodes" + legnode).c_str());

        ImGui::DragFloat3("Rotate", &rotate[i]->x);
    }
    ImGui::DragFloat3("Right_Position", &targetPos[(int)Legs::RIGHT].x);
    ImGui::DragFloat3("left_Position", &targetPos[(int)Legs::LEFT].y);
}
