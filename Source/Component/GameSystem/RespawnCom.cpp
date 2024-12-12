#include "RespawnCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Character\CharacterCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Scene\SceneTitle\SceneTitle.h"
#include "Component\Camera\EventCameraManager.h"

void RespawnCom::Update(float elapsedTime)
{
    GameObj player = GameObjectManager::Instance().Find("player");

    //���������v���C���[���E��
    if (!isRespawn && player->transform_->GetWorldPosition().y < playerDeathHeight)
    {
        auto& stateMachine = player->GetComponent<CharacterCom>()->GetMoveStateMachine();
        stateMachine.ChangeState(CharacterCom::CHARACTER_MOVE_ACTIONS::DEATH);

        isRespawn = true;
    }

    //���X�|�[������
    if (isRespawn)
    {
        switch (gameMode)
        {
        case RespawnCom::GameMode::PVE:
            //PVE�̓^�C�g���ɑJ��
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
                
                //�ŏ��ɃC�x���g�J�����֕ύX
                GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->ActiveCameraChange();

                isRespawn = false;
            }
            break;
        default:
            break;
        }
    }
}
