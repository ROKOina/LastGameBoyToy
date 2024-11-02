#include "PushBackCom.h"
#include "Component/System/TransformCom.h"
#include "Graphics/Graphics.h"

PushBackCom::PushBackCom()
{
}

void PushBackCom::Start()
{
}

void PushBackCom::Update(float elapsedTime)
{
}

void PushBackCom::OnGUI()
{
    ImGui::Checkbox("IsHit", &isHit);
    ImGui::DragFloat("radius", &radius_, 0.1f);
    ImGui::DragFloat("weight", &weight_, 0.1f);
}

void PushBackCom::DebugRender()
{
    Graphics::Instance().GetDebugRenderer()->DrawSphere(GetGameObject()->transform_->GetWorldPosition(), radius_, { 0,1,1,1 });
}

void PushBackCom::PushBackUpdate(std::shared_ptr<PushBackCom> otherSide)
{
    //必要なパラメーター取得
    DirectX::XMFLOAT3 myPos = GetGameObject()->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 otherPos = otherSide->GetGameObject()->transform_->GetWorldPosition();
    DirectX::XMVECTOR MyPos = DirectX::XMLoadFloat3(&myPos);
    DirectX::XMVECTOR OtherPos = DirectX::XMLoadFloat3(&otherPos);

    //距離求める
    float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(MyPos, OtherPos)));
    float radius = radius_ + otherSide->radius_;

    //判定
    if (dist < radius)
    {
        //重さ取得
        float myWeight = weight_;
        float otherWeight = otherSide->weight_;

        //押し返し処理
        float inSphere = radius - dist; //めり込み量
        //比率算出
        float allRatio = myWeight + otherWeight;
        float myRatio = otherWeight / allRatio;
        float otherRatio = myWeight / allRatio;

        //押し出し
        DirectX::XMVECTOR OtherFromMyNormVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(MyPos, OtherPos));
        DirectX::XMFLOAT3 myPlus;
        DirectX::XMStoreFloat3(&myPlus, DirectX::XMVectorScale(OtherFromMyNormVec, inSphere * myRatio));
        DirectX::XMFLOAT3 otherPlus;
        DirectX::XMStoreFloat3(&otherPlus, DirectX::XMVectorScale(OtherFromMyNormVec, -inSphere * otherRatio));

        myPos = { myPos.x + myPlus.x,myPos.y + myPlus.y,myPos.z + myPlus.z };
        GetGameObject()->transform_->SetWorldPosition(myPos);
        otherPos = { otherPos.x + otherPlus.x,otherPos.y + otherPlus.y,otherPos.z + otherPlus.z };
        otherSide->GetGameObject()->transform_->SetWorldPosition(otherPos);

        isHit = true;
        otherSide->isHit = true;
    }
}