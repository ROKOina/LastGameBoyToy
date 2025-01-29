#include "HitProcessCom.h"

#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\CharacterCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Sprite\Sprite.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include "Phsix\Physxlib.h"

void HitProcessCom::Update(float elapsedTime)
{
    isHit = false;
    isHitNonChara = false;

    //ヒット間隔
    hitIntervalTimer += elapsedTime;
    if (hitIntervalTimer < hitIntervalTime)return;

    //ヒットを送信
    std::shared_ptr<Collider> col = GetGameObject()->GetComponent<Collider>();
    if (!col)return;
    if (!myObj.lock())return;
    std::shared_ptr<CharacterCom> chara = myObj.lock()->GetComponent<CharacterCom>();
    if (!chara)return;

    //レイの場合建物との判定をする
    auto& ray = GetGameObject()->GetComponent<RayColliderCom>();
    if (ray)
    {
        float dist = 10000.0f;
        //一番近い距離を探す
        DirectX::XMFLOAT3 start = ray->GetStart();
        DirectX::XMFLOAT3 end = ray->GetEnd();
        for (auto& hit : col->OnHitGameObject())
        {
            float d = Mathf::Length(start - hit.hitPos);
            if (dist > d)
                dist = d;
        }
        //建物との当たりと比較
        PxRaycastBuffer buffer;
        if (PhysXLib::Instance().RayCast_PhysX(start, Mathf::Normalize(end - start), Mathf::Length(end - start), buffer, PhysXLib::CollisionLayer::Stage))
        {
            DirectX::XMFLOAT3 hitPos;
            hitPos.x = buffer.block.position.x;
            hitPos.y = buffer.block.position.y;
            hitPos.z = buffer.block.position.z;

            //建物の方が近い場合はダメージ処理を飛ばす
            float d = Mathf::Length(start - hitPos);
            if (dist > d)return;
        }
    }

    for (auto& hit : col->OnHitGameObject())
    {
        //ウルトゲージ溜める
        float ultGauge = *chara->GetUltGauge();
        ultGauge += 5;
        chara->SetUltGauge(ultGauge);

        auto& stats = hit.gameObject.lock()->GetComponent<CharaStatusCom>();
        if (!stats)continue;

        //キャラクター以外の場合
        auto& hitChara = hit.gameObject.lock()->GetComponent<CharacterCom>();
        if (!hitChara)
        {
            isHitNonChara = true;
            nonCharaObj = hit.gameObject;

            ////無敵時間の間はヒットを与えない
            //if (!stats->IsInvincible())
            //    chara->SetIsHitAttack(true);

            //ダメージを与える
            stats->AddDamagePoint(-value, -1);

            chara->SetIsHitAttack(true);

            continue;
        }

        chara->SetIsHitAttack(true);

        //ヒット処理
        HitProcess(chara->GetNetCharaData().GetNetPlayerID(), hitChara->GetNetCharaData().GetNetPlayerID());

        hitIntervalTimer = 0;

        isHit = true;
    }
}

void HitProcessCom::OnGUI()
{
    ImGui::DragFloat("hitIntervalTime", &hitIntervalTime, 0.1f, 0, 5);
    float hitT = hitIntervalTimer;
    ImGui::DragFloat("hitIntervalTimer", &hitT);

    ImGui::DragFloat("value", &value, 0.1f, 0, 100);
}

void HitProcessCom::HitProcess(int myID, int hitID)
{
    switch (hitType)
    {
    case HitProcessCom::HIT_TYPE::DAMAGE:
        StaticSendDataManager::Instance().SetSendDamage(myID, hitID, value);
        break;
    case HitProcessCom::HIT_TYPE::HEAL:
        StaticSendDataManager::Instance().SetSendHeal(myID, hitID, value);
        break;
    case HitProcessCom::HIT_TYPE::STAN:
        StaticSendDataManager::Instance().SetSendStan(myID, hitID, value);
        break;
    case HitProcessCom::HIT_TYPE::KNOCKBACK:
        StaticSendDataManager::Instance().SetSendKnockback(myID, hitID, value3);
        break;
    default:
        break;
    }
}