#include "MissileCom.h"
#include "Component\System\TransformCom.h"
#include "Audio/Audio3D.h"

void MissileCom::Start()
{
    bossSE = std::make_shared<BossCom>();
}

void MissileCom::Update(float elapsedTime)
{
    GameObj boss = GameObjectManager::Instance().Find("BOSS");
    bossSE = boss->GetComponent<BossCom>();

    bossSE->GetAudios(BOSS_BULLET)->Audio3DStop();
    bossSE->GetAudios(BOSS_BULLET)->SetVolume(10.0f);
    bossSE->GetAudios(BOSS_BULLET)->AudioPlay();
}