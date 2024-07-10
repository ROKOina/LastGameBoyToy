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
    //ターゲットポジションを取得
    targetPos[(int)Legs::RIGHT] = GetTargetPosition(Legs::RIGHT);
    targetPos[(int)Legs::LEFT] = GetTargetPosition(Legs::LEFT);

    DebugRenderer* debug = Graphics::Instance().GetDebugRenderer();
    debug->DrawSphere(targetPos[(int)Legs::LEFT], 0.05f, { 1,0,0,1 });
    debug->DrawSphere(targetPos[(int)Legs::RIGHT], 0.05f, { 1,0,0,1 });

}

//ターゲットポジション取得
DirectX::XMFLOAT3 FootIKCom::GetTargetPosition(Legs leg)
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
    float offset = 100;
    end *= offset;

    DirectX::XMFLOAT3 hitPos;
    RayCastManager::Instance().RayCast(
        start,
        end,
        hitPos);

    debug->DrawSphere(start, 0.05f, { 1,0,0,1 });


    //レイキャストの結果を返す
    return hitPos;
}

void FootIKCom::MoveBone(Legs leg)
{
    DirectX::XMFLOAT3  waistBone;   //腰
    DirectX::XMFLOAT3  knessBone;   //膝
    DirectX::XMFLOAT3  ankletBone;  //足首

    //各ボーンのワールド座標を取得
    //右足
    if ((int)leg == (int)Legs::RIGHT)
    {
        waistBone = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_WAIST]->worldTransform);
        knessBone = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);
        ankletBone = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform);
       
    }
    //左足
    else
    {
        waistBone = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_WAIST]->worldTransform);
        knessBone = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
        ankletBone = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform);
    }
    
    //ベクトルを算出
    DirectX::XMFLOAT3 waistKnessVec  = knessBone  - waistBone;   //腰から膝のベクトル
    DirectX::XMFLOAT3 knessaAnkleVec = ankletBone - knessBone;  //膝から足首のベクトル
    DirectX::XMFLOAT3 waistTargetVec;  //足からターゲットのベクトル
    if ((int)leg == (int)Legs::RIGHT)
    {
        waistTargetVec = targetPos[(int)Legs::RIGHT] - waistBone;
    }
    else
    {
        waistTargetVec = targetPos[(int)Legs::LEFT] - waistBone;
    }

    //長さを求める
    float waistKnessLength  = Mathf::Length(waistKnessVec);
    float knessaAnkleLength = Mathf::Length(knessaAnkleVec);
    float waistTargetLength = Mathf::Length(waistTargetVec);



    
    float addLength = waistKnessLength + knessaAnkleLength;
    //先端ボーン座標がターゲット座標に近づくように根本ボーンと中間ボーンを回転制御する
    {
        auto rotateBone = [](Model::Node& bone, const DirectX::XMFLOAT3& Direction1, const DirectX::XMFLOAT3& Direction2)
            {
                //回転軸算出
                DirectX::XMFLOAT3 WorldAixs = Mathf::Cross(Direction1,Direction2);
                if (Mathf::Equal(WorldAixs, {0,0,0})) return;
                
       

                //回転軸をローカル空間変換
                DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&bone.worldTransform);
                DirectX::XMMATRIX InverseParentWorldTransform = DirectX::XMMatrixInverse(nullptr, ParentWorldTransform);
                DirectX::XMVECTOR LocalAxis = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&WorldAixs), InverseParentWorldTransform);
                LocalAxis = DirectX::XMVector3Normalize(LocalAxis);

                //角度を算出
                float dot = Mathf::Dot(Direction1, Direction2);
                float angle = acosf(dot);

                //回転クォータニオン算出
                DirectX::XMVECTOR LocalRotationAxis = DirectX::XMQuaternionRotationAxis(LocalAxis, angle);
                //ここもしかしたらlocaltransformからrotate取らなきゃいけないかも？
                DirectX::XMVECTOR LocalRotation = DirectX::XMLoadFloat4(&bone.rotate);
                LocalRotation = DirectX::XMQuaternionMultiply(LocalRotation, LocalRotationAxis);
                DirectX::XMStoreFloat4(&bone.rotate, LocalRotation);
            };
        //各ベクトルを単位ベクトル化
        DirectX::XMFLOAT3 waistTargetDirection = Mathf::Normalize(waistTargetVec);
        DirectX::XMFLOAT3 knessaAnkleDirection = Mathf::Normalize(knessaAnkleVec);
        DirectX::XMFLOAT3 waistKnessDirection  = Mathf::Normalize(waistKnessVec);

        //ターゲット方向に回転させる
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
            if ((int)leg == (int)Legs::RIGHT)
            {
                legNodes[(int)LegNodes::RIGHT_WAIST]->
            }
            
        }
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
