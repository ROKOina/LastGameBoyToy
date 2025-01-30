#include "ButtonCom.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Character/CharacterCom.h"
#include "Component\GameSystem\RespawnCom.h"
#include "Netwark/Photon/StaticSendDataManager.h"

//初期化
ButtonCom::ButtonCom()
{
}

// 開始処理
void ButtonCom::Start()
{
}

// 更新処理
void ButtonCom::Update(float elapsedTime)
{
    //ここで更新
    ButtonTouch(elapsedTime);
}

// GUI描画
void ButtonCom::OnGUI()
{
}

//ボタンに触れる処理
void ButtonCom::ButtonTouch(float elapsedTime)
{
    nonPushTimer -= elapsedTime;
    if (nonPushTimer > 0)return;    //ボタン押せなくするタイマー

    //取得
    auto& collider = GetGameObject()->GetComponent<Collider>();
    for (auto& c : collider->OnHitGameObject())
    {
        //自分だけが触れる様に
        if (std::strcmp(c.gameObject.lock()->GetName(), "player") == 0)
        {
            StaticSendDataManager::Instance().SetButton();
            pushCount += 1;
            nonPushTimer = 3;
            RespawnCom* respawn = GameObjectManager::Instance().Find("respawn")->GetComponent<RespawnCom>().get();
            if (respawn)
            {
                respawn->AddRespawnData(c.gameObject.lock());
            }
        }
    }
}