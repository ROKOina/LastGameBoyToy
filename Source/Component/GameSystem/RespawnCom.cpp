#include "RespawnCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Character\CharacterCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Scene\SceneTitle\SceneTitle.h"
#include "Component\Camera\EventCameraManager.h"

void RespawnCom::Update(float elapsedTime)
{
    GameObj player = GameObjectManager::Instance().Find("player");

    //落下したプレイヤーを殺す
    if (!isRespawn && player->transform_->GetWorldPosition().y < playerDeathHeight)
    {
        auto& stateMachine = player->GetComponent<CharacterCom>()->GetMoveStateMachine();
        stateMachine.ChangeState(CharacterCom::CHARACTER_MOVE_ACTIONS::DEATH);

        isRespawn = true;
    }

    //リスポーン処理
    if (isRespawn)
    {
        switch (gameMode)
        {
        case RespawnCom::GameMode::PVE:
            //PVEはタイトルに遷移
            SceneManager::Instance().ChangeSceneDelay(new SceneTitle, 3);
            break;

        case RespawnCom::GameMode::DeathMatch:
            if (!EventCameraManager::Instance().GetIsPlayEvent())
            {
                CharacterCom* charaCom = player->GetComponent<CharacterCom>().get();

                int spawnIndex = 0;
                spawnIndex = charaCom->GetNetCharaData().GetNetPlayerID();

                if (spawnIndex < 0) { spawnIndex = 0; }
                player->transform_->SetWorldPosition(respawnPoses[spawnIndex]);

                auto& moveStateMachine = charaCom->GetMoveStateMachine();
                auto& attackStateMachine = charaCom->GetAttackStateMachine();
                moveStateMachine.ChangeState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);                
                attackStateMachine.ChangeState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
                
                //最初にイベントカメラへ変更
                GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->ActiveCameraChange();

                isRespawn = false;
            }
            break;
        default:
            break;
        }
    }
}
