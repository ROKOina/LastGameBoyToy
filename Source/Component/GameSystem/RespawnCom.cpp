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
        CharaStatusCom* status = player->GetComponent<CharaStatusCom>().get();
        status->AddDamagePoint(-200);
    }

    //リスポーン処理
    for (RespawnData* respawnData : respawnDatas)
    {
        respawnData->respawnTime += elapsedTime;

        //死亡演出が終了したらリスポーン
        if (respawnData->respawnTime >= 2.5f)
        {
            CharacterCom* charaCom = respawnData->gameObj->GetComponent<CharacterCom>().get();

            //プレイヤーならば
            if (std::string(respawnData->gameObj->GetName()) == "player")
            {
                //位置移動
                int spawnIndex = 0;
                spawnIndex = charaCom->GetNetCharaData().GetNetPlayerID();
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

            //パラメータ回復
            CharaStatusCom* status = respawnData->gameObj->GetComponent<CharaStatusCom>().get();
            status->SetIsDeath(false);

            //アニメーションを死亡から待機へ
            AnimationCom* animaCom = player->GetComponent<AnimationCom>().get();
            animaCom->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
            animaCom->PlayUpperBodyOnlyAnimation(animaCom->FindAnimation("Idle"), true, 1.0f);
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

    //リスポーン終了したオブジェクトをコンテナから出す
    for (RespawnData* removeObj : endDatas)
    {
        respawnDatas.erase(std::remove(respawnDatas.begin(), respawnDatas.end(), removeObj), respawnDatas.end());
    }
    endDatas.clear();
}
