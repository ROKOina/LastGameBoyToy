#include "RespawnCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Character\CharacterCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Scene\SceneTitle\SceneTitle.h"
#include "Component\Camera\EventCameraManager.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Animation\AnimationCom.h"

void RespawnCom::Update(float elapsedTime)
{
    GameObj player = GameObjectManager::Instance().Find("player");

    //落下したプレイヤーを殺す
    if (!fallEvent && player->transform_->GetWorldPosition().y < playerDeathHeight)
    {
        auto& stateMachine = player->GetComponent<CharacterCom>()->GetMoveStateMachine();
        CharaStatusCom* status = player->GetComponent<CharaStatusCom>().get();
        status->AddDamagePoint(-200);

        //stateMachine.ChangeState(CharacterCom::CHARACTER_MOVE_ACTIONS::DEATH);

        //fallEvent = true;
    }

    //リスポーン処理
    for (auto& respawnData : respawnDatas)
    {
        respawnData.respawnTime += elapsedTime;

        if (respawnTimer >= 2.5f)
        {
            respawnData.gameObj;

            CharacterCom* charaCom = player->GetComponent<CharacterCom>().get();

            //プレイヤーならば
            if (std::string(respawnData.gameObj->GetName()) == "player")
            {
                //プレイヤーリスポーン処理
                int spawnIndex = 0;
                spawnIndex = charaCom->GetNetCharaData().GetNetPlayerID();

                //位置
                if (spawnIndex < 0) { spawnIndex = 0; }
                player->transform_->SetWorldPosition(respawnPoses[spawnIndex]);
                //パラメータ回復
                CharaStatusCom* status = player->GetComponent<CharaStatusCom>().get();
                status->ReSpawn(status->GetMaxHitpoint());

                //プレイヤー隠す
                player->GetComponent<RendererCom>()->SetDissolveThreshold(1);
                //FPS用オブジェクト映す
                GameObjectManager::Instance().Find("armChild")->GetComponent<RendererCom>()->SetDissolveThreshold(0);

                //最初にイベントカメラへ変更
                GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->ActiveCameraChange();

            }

            
            AnimationCom* animaCom = player->GetComponent<AnimationCom>().get();
            animaCom->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
            animaCom->PlayAnimation(animaCom->FindAnimation("Idle"), true, 1.0f);

            for (int i = 0; i < 20; ++i)
            {
                animaCom->Update(elapsedTime);
            }

            //ステートを通常に戻す
            auto& moveStateMachine = charaCom->GetMoveStateMachine();
            auto& attackStateMachine = charaCom->GetAttackStateMachine();
            moveStateMachine.ChangeState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
            attackStateMachine.ChangeState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);

           
            endDatas.emplace_back(respawnData);
        }


    }

    if (isRespawn)
    {
        switch (gameMode)
        {
        case PVPGameSystem::GAME_MODE::None:
            //PVEはタイトルに遷移
            SceneManager::Instance().ChangeSceneDelay(new SceneTitle, 3);
            break;

        case PVPGameSystem::GAME_MODE::Deathmatch:

            respawnTimer += elapsedTime;

            if (respawnTimer >= 2.5f)//(!EventCameraManager::Instance().GetIsPlayEvent())
            {
                CharacterCom* charaCom = player->GetComponent<CharacterCom>().get();

                int spawnIndex = 0;
                spawnIndex = charaCom->GetNetCharaData().GetNetPlayerID();

                //プレイヤーリスポーン処理

                //位置
                if (spawnIndex < 0) { spawnIndex = 0; }
                player->transform_->SetWorldPosition(respawnPoses[spawnIndex]);

                //パラメータ回復
                CharaStatusCom* status = player->GetComponent<CharaStatusCom>().get();
                status->ReSpawn(status->GetMaxHitpoint());

                AnimationCom* animaCom = player->GetComponent<AnimationCom>().get();
                animaCom->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
                animaCom->PlayAnimation(animaCom->FindAnimation("Idle"), true, 1.0f);

                for (int i = 0; i < 20; ++i)
                {
                    animaCom->Update(elapsedTime);
                }

                //ステートを通常に戻す
                auto& moveStateMachine = charaCom->GetMoveStateMachine();
                auto& attackStateMachine = charaCom->GetAttackStateMachine();
                moveStateMachine.ChangeState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);                
                attackStateMachine.ChangeState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);

                //プレイヤー隠す
                player->GetComponent<RendererCom>()->SetDissolveThreshold(1);
                //FPS用オブジェクト映す
                GameObjectManager::Instance().Find("armChild")->GetComponent<RendererCom>()->SetDissolveThreshold(0);

                //最初にイベントカメラへ変更
                GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->ActiveCameraChange();

                respawnTimer = 0.0f;
                isRespawn = false;
            }
            break;
        default:
            break;
        }
    }
}
