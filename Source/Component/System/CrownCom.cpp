#include "CrownCom.h"

#include "Component/System/TransformCom.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Character/CharaStatusCom.h"

void CrownCom::Update(float elapsedTime)
{
    if (haveObj.lock())
        IsCrownHaveUpdate(elapsedTime);
    else
        IsCrownNonHaveUpdate(elapsedTime);
}

void CrownCom::OnGUI()
{
    bool h = HaveCrown();
    ImGui::Checkbox("myHaveCrown", &h);

    ImGui::Checkbox("haveCrownObject", &haveCrownObject);

    ImGui::DragFloat("haveTimer", &haveTimer);

    ImGui::DragFloat("secondGetTimer", &secondGetTimer);

}

bool CrownCom::HaveCrown()
{
    if(!haveObj.lock())
        return false;

    if (std::strcmp(haveObj.lock()->GetName(), "player") == 0)
    {
        return true;
    }
    return false;
}

DirectX::XMFLOAT3 CrownCom::GetLastPos()
{
    DirectX::XMFLOAT3 pos = {};
    if (HaveCrown()) //所持中
        haveCrownObject = true;
    else
    {
        if (haveCrownObject)    //手放した次のフレーム
        {
            pos = lastPos;
        }
        haveCrownObject = false;
    }

    return pos;
}

void CrownCom::GetCrown(std::shared_ptr<GameObject> obj)
{
    haveObj = obj;
}

void CrownCom::DelCrown()
{
    haveObj.reset();
}

bool CrownCom::RegisteredCrown()
{
    return bool(haveObj.lock());
}

void CrownCom::IsCrownHaveUpdate(float elapsedTime)
{
    //自分が所持している場合
    if (std::strcmp(haveObj.lock()->GetName(), "player") == 0)
    {
        //死亡時は手放す
        if (haveObj.lock()->GetComponent<CharaStatusCom>()->IsDeath())
        {
            secondGetTimer = 4;
            DelCrown();
            return;
        }
        //時間
        haveTimer += elapsedTime;
    }

    //位置
    DirectX::XMFLOAT3 pos = haveObj.lock()->transform_->GetWorldPosition();
    lastPos = pos;  //所持中の位置を保存
    pos.y += 1.5f;
    GetGameObject()->transform_->SetWorldPosition(pos);
}

void CrownCom::IsCrownNonHaveUpdate(float elapsedTime)
{
    secondGetTimer -= elapsedTime;
    if (secondGetTimer > 0)return;

    //取得
    auto& col = GetGameObject()->GetComponent<Collider>();
    for (auto& c : col->OnHitGameObject())
    {
        //プレイヤーのみ取得できる
        if (std::strcmp(c.gameObject.lock()->GetName(), "player") == 0)
        {
            haveObj = c.gameObject; //取得
        }
    }

    //クラウン落下時
    if (GetGameObject()->transform_->GetWorldPosition().y < playerDeathHeight)
    {
        //初期位置に戻す
        GetGameObject()->transform_->SetWorldPosition({ 3.4f, 20, 8.1f });
    }
}
