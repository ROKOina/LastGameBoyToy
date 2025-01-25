#include "MissileCom.h"
#include "Component\System\TransformCom.h"
#include "Audio/Audio3D.h"

void MissileCom::Start()
{
}

void MissileCom::Update(float elapsedTime)
{
    GameObj player = GameObjectManager::Instance().Find("player");

    if (!playFlag)
    {
        //ƒvƒŒƒCƒ„[‚Æ‚Ì‹——£‚É‰ž‚¶‚Ä‰¹‚ð‘å‚«‚­
        float len = Mathf::Length(player->transform_->GetWorldPosition() - GetGameObject()->transform_->GetWorldPosition());
        if (len < soundLen)
        {
            Audio2DMagaer::Instance().Audio2DPlay(AUDIOID2D::BOSS_BULLET, false, 3.0f);
            Audio2DMagaer::Instance().Audio2DFeed(AUDIOID2D::BOSS_BULLET, 10.0f, 0.2f);
            playFlag = true;
        }
    }
}