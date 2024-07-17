#include "FootIKCom.h"
#include "GameSource\Math\Collision.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"
#include "Components\System\RayCastManager.h"]
#include "Graphics\Graphics.h"

#define Float4x4ToFloat3(transform) DirectX::XMFLOAT3(transform._41,transform._42,transform._43)

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
            }
        }
    }
    stageModel = GameObjectManager::Instance().Find("stage")->GetComponent<RendererCom>()->GetModel();
}

void FootIKCom::Update(float elassedTime)
{
    //Sleep(10 * 60);
    DebugRenderer* debug = Graphics::Instance().GetDebugRenderer();

    // //ターゲットポジションを取得
     if (GetTargetPosition(Legs::RIGHT, targetPos[(int)Legs::RIGHT])) {
         MoveBone(Legs::RIGHT);
     }
     
     if (GetTargetPosition(Legs::LEFT, targetPos[(int)Legs::LEFT])) {
         MoveBone(Legs::LEFT);
     }
   
    debug->DrawSphere(targetPos[(int)Legs::RIGHT], 0.05f, { 1,0,0,1 });
    debug->DrawSphere(targetPos[(int)Legs::LEFT], 0.05f, { 0,0,1,1 });
}

//ターゲットポジション取得
bool FootIKCom::GetTargetPosition(Legs leg, DirectX::XMFLOAT3& resultPos)
{
    //デバック描画
    DebugRenderer* debug = Graphics::Instance().GetDebugRenderer();

    //レイの始点、終点
    DirectX::XMFLOAT3 start;
    DirectX::XMFLOAT3 end;

    //右足のターゲットポジション
    if ((int)leg == (int)Legs::RIGHT)
    {
        //膝からレイを飛ばす
        start = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);
        //膝から足首のベクトル
        end = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform) - Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);

    }
    //左足のターゲットポジション
    else
    {
        //膝からレイを飛ばす
        start = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
        //膝から足首のベクトル
        end = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform) - Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
    }

    //正規化
    end = Mathf::Normalize(end);

    //オフセット分レイを伸ばす
    float offset = 0.7f;
    end *= offset;

    end += start;

    auto line = Graphics::Instance().GetLineRenderer();
    if ((int)leg == (int)Legs::RIGHT) {
        line->AddVertex(start, { 0,1,1,1 });
        line->AddVertex(end, { 0,1,1,1 });
    }
    else {
        line->AddVertex(start, { 1,1,0,1 });
        line->AddVertex(end, { 1,1,0,1 });
    }

    bool result = RayCastManager::Instance().RayCast(
        start,
        end,
        resultPos);

    debug->DrawSphere(start, 0.05f, { 1,0,1,1 });

    if (!result) {
      
        //targetPos[(int)leg] = end;
    }

    //レイキャストの結果を返す
    return result;
}

void FootIKCom::MoveBone(Legs leg)
{
    DirectX::XMFLOAT3  waistBonePosition;   //腰
    DirectX::XMFLOAT3  knessBonePosition;   //膝
    DirectX::XMFLOAT3  ankletBonePosition;  //足首

    //各ボーンのワールド座標を取得
    //右足
    if ((int)leg == (int)Legs::RIGHT)
    {
        waistBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_WAIST]->worldTransform);
        knessBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);
        ankletBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform);

    }
    //左足
    else
    {
        waistBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_WAIST]->worldTransform);
        knessBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
        ankletBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform);
    }

    //ベクトルを算出
    DirectX::XMFLOAT3 waistKnessVec = knessBonePosition - waistBonePosition;   //腰から膝のベクトル
    DirectX::XMFLOAT3 knessAnkleVec = ankletBonePosition - knessBonePosition;  //膝から足首のベクトル
    DirectX::XMFLOAT3 waistTargetVec;  //足からターゲットのベクトル
    if ((int)leg == (int)Legs::RIGHT)
    {
        waistTargetVec = targetPos[(int)Legs::RIGHT] - waistBonePosition;
    }
    else
    {
        waistTargetVec = targetPos[(int)Legs::LEFT] - waistBonePosition;
    }

    //長さを求める
    float waistKnessLength = Mathf::Length(waistKnessVec);
    float knessAnkleLength = Mathf::Length(knessAnkleVec);
    float waistTargetLength = Mathf::Length(waistTargetVec);

    float addLength = waistKnessLength + knessAnkleLength;

    //先端ボーン座標がターゲット座標に近づくように根本ボーンと中間ボーンを回転制御する
    {
        auto rotateBone = [](Model::Node& bone, const DirectX::XMFLOAT3& Direction1, const DirectX::XMFLOAT3& Direction2)
            {
                //回転軸算出
                DirectX::XMFLOAT3 WorldAixs = Mathf::Cross(Direction1, Direction2);
                if (Mathf::Equal(WorldAixs, { 0,0,0 }))
                {
                    return;
                }
                if (Mathf::Equal(Direction1, Direction2))
                {
                    return;
                }

                //回転軸をローカル空間変換
                DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&bone.worldTransform);
                DirectX::XMMATRIX InverseParentWorldTransform = DirectX::XMMatrixInverse(nullptr, ParentWorldTransform);
                DirectX::XMVECTOR LocalAxis = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&WorldAixs), InverseParentWorldTransform);
                LocalAxis = DirectX::XMVector3Normalize(LocalAxis);

                //角度を算出
                float dot = Mathf::Dot(Direction1, Direction2);
                if (dot == 0)
                {
                    return;
                }
                float angle = acosf(dot);

               //回転クォータニオン算出
               DirectX::XMVECTOR LocalRotationAxis = DirectX::XMQuaternionRotationAxis(LocalAxis, angle);
               DirectX::XMVECTOR LocalRotation = DirectX::XMLoadFloat4(&bone.rotate);
               LocalRotation = DirectX::XMQuaternionMultiply(LocalRotation, LocalRotationAxis);
               DirectX::XMStoreFloat4(&bone.rotate, LocalRotation);
                
            };

        //各ベクトルを単位ベクトル化
        DirectX::XMFLOAT3 waistTargetDirection = Mathf::Normalize(waistTargetVec);
        DirectX::XMFLOAT3 knessAnkleDirection = Mathf::Normalize(knessAnkleVec);
        DirectX::XMFLOAT3 waistKnessDirection = Mathf::Normalize(waistKnessVec);

        //ターゲット方向に回転させる
        //腰の回転
        if ((int)leg == (int)Legs::RIGHT)
        {
           rotateBone(*legNodes[(int)LegNodes::RIGHT_WAIST], waistKnessDirection, waistTargetDirection);
        }
        else
        {
           rotateBone(*legNodes[(int)LegNodes::LEFT_WAIST], waistKnessDirection, waistTargetDirection);
        }

        if (addLength <= waistTargetLength)
        {
           //ターゲット方向に回転させる
           //膝の回転
           if ((int)leg == (int)Legs::RIGHT)
           {
               UpdateWorldTransform(legNodes[(int)LegNodes::RIGHT_WAIST]);
               DirectX::XMFLOAT3 kneesTargetVec = targetPos[(int)Legs::RIGHT] - knessBonePosition;
               DirectX::XMFLOAT3 kneestTargetDirection = Mathf::Normalize(kneesTargetVec);
               rotateBone(*legNodes[(int)LegNodes::RIGHT_KNEES], knessAnkleDirection, kneestTargetDirection);
           }
           else
           {
               UpdateWorldTransform(legNodes[(int)LegNodes::LEFT_WAIST]);
               DirectX::XMFLOAT3 kneesTargetVec = targetPos[(int)Legs::LEFT] - knessBonePosition;
               DirectX::XMFLOAT3 kneestTargetDirection = Mathf::Normalize(kneesTargetVec);
               rotateBone(*legNodes[(int)LegNodes::LEFT_KNEES], knessAnkleDirection, kneestTargetDirection);
           }
        }

        if (addLength > waistTargetLength)
        {
         //三つのベクトルで面積を算出
         float s = (addLength + waistTargetLength) / 2;
         s = sqrtf(s * (s - waistKnessLength) * (s - knessAnkleLength) * (s - waistTargetLength));
         //算出した面積で高さを算出
         float t = s * 2 / waistKnessLength;
         //直角三角形の斜辺と高さから角度を算出
         float root = asinf(t / waistTargetLength);
        }

        if ((int)leg == (int)Legs::RIGHT)
        {
          UpdateWorldTransform(legNodes[(int)LegNodes::RIGHT_WAIST]);
          knessBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);
          ankletBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform);
          
          DirectX::XMFLOAT3 knessTargetVec = targetPos[(int)Legs::RIGHT] - knessBonePosition;
          DirectX::XMFLOAT3 knessTargetDirection = Mathf::Normalize(knessTargetVec);
          knessAnkleVec = ankletBonePosition - knessBonePosition;
          knessAnkleDirection = Mathf::Normalize(knessAnkleDirection);
          rotateBone(*legNodes[(int)LegNodes::RIGHT_KNEES], knessAnkleDirection, knessTargetDirection);
          UpdateWorldTransform(legNodes[(int)LegNodes::RIGHT_ANKLE]);

        }
        else
        {
           UpdateWorldTransform(legNodes[(int)LegNodes::LEFT_WAIST]);
           knessBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
           ankletBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform);
           
           DirectX::XMFLOAT3 knessTargetVec = targetPos[(int)Legs::LEFT] - knessBonePosition;
           DirectX::XMFLOAT3 knessTargetDirection = Mathf::Normalize(knessTargetVec);
           knessAnkleVec = ankletBonePosition - knessBonePosition;
           knessAnkleDirection = Mathf::Normalize(knessAnkleDirection);
           rotateBone(*legNodes[(int)LegNodes::LEFT_KNEES], knessAnkleDirection, knessTargetDirection);
           UpdateWorldTransform(legNodes[(int)LegNodes::LEFT_ANKLE]);
        }
    }
}

void FootIKCom::UpdateWorldTransform(Model::Node* legNode)
{
    Model::Node* node = legNode;
    DirectX::XMMATRIX LocalRotationTransform = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node->rotate));
    DirectX::XMMATRIX LocalPositionTransform = DirectX::XMMatrixTranslation(node->translate.x, node->translate.y, node->translate.z);
    DirectX::XMMATRIX LocalTransform = DirectX::XMMatrixMultiply(LocalRotationTransform, LocalPositionTransform);
    if (node->parent != nullptr)
    {
        DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&node->parent->worldTransform);
        DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(LocalTransform, ParentWorldTransform);
        DirectX::XMStoreFloat4x4(&node->worldTransform, WorldTransform);
    }
    else
    {
        DirectX::XMStoreFloat4x4(&node->worldTransform, LocalTransform);
    }

}



void FootIKCom::OnGUI()
{
    for (int i = 0; i < 6; i++)
    {
        std::string legnode = legNodes[i]->name;
        ImGui::Text(std::string(legnode).c_str());
        DirectX::XMFLOAT3 pos = Float4x4ToFloat3(legNodes[i]->worldTransform);
        ImGui::DragFloat3("Rotate", &pos.x);
    }
    ImGui::Separator();
    ImGui::DragFloat3("Right_Position", &targetPos[(int)Legs::RIGHT].x);
    ImGui::DragFloat3("Left_Position", &targetPos[(int)Legs::LEFT].x);
}
