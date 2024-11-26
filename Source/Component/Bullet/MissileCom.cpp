#include "MissileCom.h"
#include "Component\Audio\AudioCom.h"
#include "Component\System\TransformCom.h"

void MissileCom::Start()
{
    AudioCom* audio = GetGameObject()->GetComponent<AudioCom>().get();
    audio->RegisterSource(AUDIOID::BOSS_BULLET, "FIRE");
}

void MissileCom::Update(float elapsedTime)
{
    GameObj player = GameObjectManager::Instance().Find("player");
    AudioCom* audio = GetGameObject()->GetComponent<AudioCom>().get();

    if (!playFlag)
    {
        //ƒvƒŒƒCƒ„[‚Æ‚Ì‹——£‚É‰ž‚¶‚Ä‰¹‚ð‘å‚«‚­
        float len = Mathf::Length(player->transform_->GetWorldPosition() - GetGameObject()->transform_->GetWorldPosition());
        if (len < soundLen)
        {
            audio->Play("FIRE", false, 3.0f);
            audio->FeedStart("FIRE", 10.0f, 0.2f);
            playFlag = true;
        }
    }
}