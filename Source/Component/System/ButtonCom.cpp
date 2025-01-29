#include "ButtonCom.h"
#include "Component/Collsion/ColliderCom.h"

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
    ButtonTouch();
}

// GUI描画
void ButtonCom::OnGUI()
{
}

//ボタンに触れる処理
void ButtonCom::ButtonTouch()
{
    //取得
    auto& collider = GetGameObject()->GetComponent<Collider>();
    for (auto& c : collider->OnHitGameObject())
    {
        //自分と味方だけが触れる様にしたい
        if (std::strcmp(c.gameObject.lock()->GetName(), "player") == 0)
        {
            //auto& DM = pvpGameSystem.lock()->GetButtonData();
            //DM.teamData[PVPGameSystem::TEAM_KIND::RED_GROUP].buttoncount += 1;
            //DM.teamData[PVPGameSystem::TEAM_KIND::BLUE_GROUP].buttoncount += 1;
        }
    }
}