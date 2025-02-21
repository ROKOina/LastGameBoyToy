#include "ExpoTitleControllerCom.h"

#include <Audio\Audio3D.h>
#include "Component\Renderer\VideoCom.h"
#include <Scene\SceneManager.h>
#include "Scene/SceneLoading/SceneLoading.h"
#include "Scene/ScenePVE/ScenePVE.h"
#include "Scene\ScenePVP\ScenePVP.h"
#include "Scene\SceneTraining\SceneTraining.h"
#include "Component\PostEffect\PostEffect.h"

ExpoTitleControllerCom::ExpoTitleControllerCom()
{
}

void ExpoTitleControllerCom::Start()
{
    state = INTRO;
    for (auto ui : uiObjs)
    {
        if (auto collision = ui.lock()->GetComponent<SpriteCollisionCom>())
        {
            uiCollisions.emplace_back(collision);
        }
    }
    for (auto select : selectEvents)
    {
        selectObjs.emplace_back(select.lock()->GetGameObject());
    }

    int selectId = 0;
    for (auto coll : uiCollisions)
    {
        if (auto selectEvent = selectEvents.at(selectId).lock())
        {
            coll.lock()->SetHitSpriteEnter([=]()
                {
                    Audio2DMagaer::Instance().Audio2DStop(AUDIOID2D::CURSOR);
                    Audio2DMagaer::Instance().Audio2DPlay(AUDIOID2D::CURSOR, 1.0f, false);
                    const int id = selectId;
                    // カーソルが当たった時のUI演出
                    if (selectEvent->GetPlayEnable())
                    {
                        if (selectEvent->GetReverseEnable())
                        {
                            selectEvent->SetReverseEnable(false);
                            selectEvent->EventResume();
                        }
                        else
                        {
                            selectEvent->EventResume();
                        }
                    }
                    else
                    {
                        selectEvent->EventPlay();
                    }
                }
            );
        }
        if (auto selectEvent = selectEvents.at(selectId).lock())
        {
            coll.lock()->SetHitSpriteExit([=]()
                {
                    // カーソルが離れた時のUI演出
                    if (selectEvent->GetPlayEnable())
                    {
                        if (selectEvent->GetReverseEnable())
                        {
                            selectEvent->EventResume();
                        }
                        else
                        {
                            selectEvent->EventReversePlay();
                        }
                    }
                    else
                    {
                        selectEvent->EventReversePlay();
                    }
                }
            );
        }
        selectId++;
    }
}

void ExpoTitleControllerCom::Update(float elapsedTime)
{
    switch (state)
    {
    case ExpoTitleControllerCom::INTRO:
        // イントロ演出が終るまで入力を待たせる
        if (introUiEvent.lock()->GetFinishEnable())
        {
            state = SELECT_UPDATE;
        }
        break;
    case ExpoTitleControllerCom::SELECT_UPDATE:
        SelectUpdate();
        break;
    case ExpoTitleControllerCom::CREDIT_UPDATE:
        CreditUpdate();
        break;
    case ExpoTitleControllerCom::NEXT_SCENE:
        break;
    }
}

void ExpoTitleControllerCom::CreditUpdate()
{
    auto& creditVideo = GameObjectManager::Instance().Find("CreditVideo");

    GamePad& gamePad = Input::Instance().GetGamePad();
    if (GamePad::ENTER & gamePad.GetButton())
    {
        creditVideo->GetComponent<Video>()->GetVidePram().SetTimeScale(30.0f);

    }
    else {
        creditVideo->GetComponent<Video>()->GetVidePram().SetTimeScale(1.0f);
    }

    auto Skip = [&]()
        {
            creditVideo->GetComponent<Video>()->GetVidePram().SetRestart();
            state = SELECT_UPDATE;
            selectState = SelectItem::NONE;
            UIActive(true);
            GameObjectManager::Instance().Find("stage")->SetEnabled(true);
            GameObjectManager::Instance().Find("Space")->SetEnabled(false);
            GameObjectManager::Instance().Find("Enter")->SetEnabled(false);

            Audio2DMagaer::Instance().Audio2DStop(AUDIOID2D::CREDIT);
            Audio2DMagaer::Instance().Audio2DPlay(AUDIOID2D::SCENE_TITLE, 1.0f, true);
            creditVideo->SetEnabled(false);
        };

    //スペースキーを押したらスキップ
    if (GamePad::BTN_A & gamePad.GetButtonDown())
    {
        Skip();
        return;
    }
    if (creditVideo->GetComponent<Video>()->GetVidePram().hasFinished()) {
        Skip();
    }
}

void ExpoTitleControllerCom::SelectUpdate()
{
    //遷移フラグが立っている時は入らない
    if (SceneManager::Instance().GetTransitionFlag())return;

    GamePad& gamePad = Input::Instance().GetGamePad();
    int selectId = 0;
    for (auto coll : uiCollisions)
    {
        if (coll.lock()->GetHitSprite())
        {
            if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
            {
                selectState = static_cast<SelectItem>(selectId);
            }
        }
        selectId++;
    }

    if (selectState == SelectItem::NONE)return;

    auto Prepare = [&]()
        {
            introUiEvent.lock()->EventReversePlay();

            //BGM消す
            Audio2DMagaer::Instance().Audio2DStop(AUDIOID2D::SCENE_TITLE);
            Audio2DMagaer::Instance().Audio2DPlay(AUDIOID2D::ENTER);

            //暗転
            std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
            GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(0.0f, 4.0f, parameters);

            state = NEXT_SCENE;
        };

    switch (selectState)
    {
    case ExpoTitleControllerCom::PVE:
        SceneManager::Instance().ChangeSceneDelay(new ScenePVE, 2);
        Prepare();
        break;
    case ExpoTitleControllerCom::PVP:
        SceneManager::Instance().ChangeSceneDelay(new ScenePVP, 2);
        Prepare();
        break;
    case ExpoTitleControllerCom::TRAINING:
        SceneManager::Instance().ChangeSceneDelay(new SceneTraining, 2);
        Prepare();
        break;
    case ExpoTitleControllerCom::CREDIT:
    {
        UIActive(false);
        GameObjectManager::Instance().Find("stage")->SetEnabled(false);

        Audio2DMagaer::Instance().Audio2DStop(AUDIOID2D::SCENE_TITLE);
        Audio2DMagaer::Instance().Audio2DPlay(AUDIOID2D::CREDIT, 1.0f, false);
        GameObjectManager::Instance().Find("Space")->SetEnabled(true);
        GameObjectManager::Instance().Find("Enter")->SetEnabled(true);

        auto creditVideo = GameObjectManager::Instance().Find("CreditVideo");
        creditVideo->GetComponent<Video>()->GetVidePram().SetRestart();
        creditVideo->GetComponent<Video>()->GetVidePram().resume();
        creditVideo->SetEnabled(true);
        state = CREDIT_UPDATE;
    }
        break;
    case ExpoTitleControllerCom::END_GAME:
        PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
        break;
    }
}

void ExpoTitleControllerCom::UIActive(bool enable)
{
    for (auto select : selectObjs)
    {
        select.lock()->SetEnabled(enable);
    }
    for (auto ui : uiObjs)
    {
        ui.lock()->SetEnabled(enable);
    }

}

void ExpoTitleControllerCom::OnGUI()
{
}
