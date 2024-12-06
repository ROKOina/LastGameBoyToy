#include "Component/Item/UltSkillMaxItem.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Character/CharacterCom.h"
#include "Scene/SceneTraining/TrainingManager.h"
#include "Component/System/TransformCom.h"
void UltSkillMaxItem::Start()
{

}

void UltSkillMaxItem::Update(float elapsedTime)
{
    OnItemPickup();

    TransformItem(elapsedTime);
}

void UltSkillMaxItem::OnGUI()
{

}

//アイテムを拾った時の処理
void UltSkillMaxItem::OnItemPickup()
{
    float ultGauge = *GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetUltGauge();
    if (ultGauge!=GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetUltGaugeMax())
    {
        for (auto& hit : GameObjectManager::Instance().Find("ULTSKILLMAXITEM")->GetComponent<SphereColliderCom>()->OnHitGameObject())
        {
            //プレイヤーと接触したら
            if (static_cast<std::string>(hit.gameObject.lock()->GetName()) == "player")
            {
                //アイテムの効果
                hit.gameObject.lock()->GetComponent<CharacterCom>()->SetUltGauge(100.0f);
                hit.gameObject.lock()->GetComponent<CharacterCom>()->ResetSkillCoolTimer(CharacterCom::SkillCoolID::E);
                TrainingSystem::Instance().SetItemFlag(false);
                GameObjectManager::Instance().Remove(this->GetGameObject());
            }
        }
    }
}

//アイテムの動き
void UltSkillMaxItem::TransformItem(float elapsedTime)
{
   
    DirectX::XMFLOAT3 Pos = GameObjectManager::Instance().Find("ULTSKILLMAXITEM")->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 Eule = GameObjectManager::Instance().Find("ULTSKILLMAXITEM")->transform_->GetEulerRotation();

    Pos.y = 1.0f +0.5f*std::sin(elapsedTime);
    Eule.y += 3*elapsedTime;

    GameObjectManager::Instance().Find("ULTSKILLMAXITEM")->transform_->SetEulerRotation(Eule);
    GameObjectManager::Instance().Find("ULTSKILLMAXITEM")->transform_->SetWorldPosition(Pos);
}