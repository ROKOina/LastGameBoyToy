#include "Component/Item/UltSkillMaxItem.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Character/CharacterCom.h"
#include "Scene/SceneTraining/TrainingManager.h"
void UltSkillMaxItem::Start()
{

}

void UltSkillMaxItem::Update(float elapsedTime)
{
    OnItemPickup();
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