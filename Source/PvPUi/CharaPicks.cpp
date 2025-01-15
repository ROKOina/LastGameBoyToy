#include "CharaPicks.h"
#include "Input/Input.h"
#include "Component/Sprite/Sprite.h"
#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Animation\AnimationCom.h"
#include <Component\Camera\FreeCameraCom.h>
#include "Component\Renderer\VideoCom.h"

CharaPicks::CharaPicks()
{
    color = { 1.0f, 1.0f, 1.0f, 1.0f };
    selectColor = { 0.3f, 0.3f, 0.3f, 1.0f };
}

// キャラピックUI生成
void CharaPicks::CreateCharaPicksUiObject()
{
    //キャラピック専用カメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("charapickcamera");
        freeCamera->transform_->SetWorldPosition({ 0.044f, 1.674f, -1.450f });
        freeCamera->transform_->SetEulerRotation({ -0.160f,-5.440f,0.0f });
        std::shared_ptr<FreeCameraCom> camera = freeCamera->AddComponent<FreeCameraCom>();
        camera->SetFocusPos({ -0.264f,1.683f,1.785f });
        camera->SetFocus({ -0.145f,1.680f,0.541f });
        camera->SetEye({ 0.044f,1.674f,-1.450f });
        camera->SetDistance(3.249f);
        camera->SetUpdate(false);
    }
    GameObjectManager::Instance().Find("charapickcamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    // キャラピックキャンパス
    auto& charaPicksCanvas = GameObjectManager::Instance().Create();
    charaPicksCanvas->SetName("CharaPicksCanvas");

    //各キャラのUI
    {
        // KANIZO-
        AddCharacterUI(charaPicksCanvas, "Kanizo-", "Data/SerializeData/UIData/CharaPick/charaIcon0.ui",
            "Data/SerializeData/UIData/CharaPick/charaName0.ui", "Data/SerializeData/UIData/CharaPick/kanizo_rightclickskillicon.ui",
            "Data/SerializeData/UIData/CharaPick/kanizo_Eskillicon.ui", "Data/SerializeData/UIData/CharaPick/kanizo_ultskillicon.ui",
            "Data/Model/player_True/player1.mdl", "Data/Video/inazo_Eskill.mp4", "Data/Video/inazo_Eskill.mp4", "Data/Video/inazo_Eskill.mp4",
            "kanizo-video1"/*, "kanizo-video2", "kanizo-ultvideo"*/);

        // FARAIC
        AddCharacterUI(charaPicksCanvas, "Faraic", "Data/SerializeData/UIData/CharaPick/charaIcon1.ui",
            "Data/SerializeData/UIData/CharaPick/charaName1.ui", "Data/SerializeData/UIData/CharaPick/farahc_rightclickskillicon.ui",
            "Data/SerializeData/UIData/CharaPick/farahc_Eskillicon.ui", "Data/SerializeData/UIData/CharaPick/farahc_ulticon.ui",
            "Data/Model/player_True/player2.mdl", "Data/Video/inazo_Eskill.mp4", "Data/Video/inazo_Eskill.mp4", "Data/Video/inazo_Eskill.mp4",
            "Faraic-video1"/*, "Faraic-video2", "Faraic-ultvideo"*/);

        // SANTORATTO
        AddCharacterUI(charaPicksCanvas, "Santoratto", "Data/SerializeData/UIData/CharaPick/charaIcon2.ui",
            "Data/SerializeData/UIData/CharaPick/charaName2.ui", "Data/SerializeData/UIData/CharaPick/santorat_rightclickskillicon.ui",
            "Data/SerializeData/UIData/CharaPick/santorat_Eskillicon.ui", "Data/SerializeData/UIData/CharaPick/santorat_ulticon.ui",
            "Data/Model/player_True/player3.mdl", "Data/Video/inazo_Eskill.mp4", "Data/Video/inazo_Eskill.mp4", "Data/Video/inazo_Eskill.mp4",
            "Santoratto-video1"/*, "Santoratto-video2", "Santoratto-ultvideo"*/);

        // MATYA-
        AddCharacterUI(charaPicksCanvas, "Matya-", "Data/SerializeData/UIData/CharaPick/charaIcon3.ui",
            "Data/SerializeData/UIData/CharaPick/charaName3.ui", "Data/SerializeData/UIData/CharaPick/matha_rightclickskillicon.ui",
            "Data/SerializeData/UIData/CharaPick/matha_Eskillicon.ui", "Data/SerializeData/UIData/CharaPick/matha_ulticon.ui",
            "Data/Model/player_True/player4.mdl", "Data/Video/inazo_Eskill.mp4", "Data/Video/inazo_Eskill.mp4", "Data/Video/inazo_Eskill.mp4",
            "Matya-video1"/*, "Matya-video2", "Matya-ultvideo"*/);
    }

    // 決定ボタン
    {
        auto& decision = charaPicksCanvas->AddChildObject();
        decision->SetName("decision");
        decision->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/decision.ui", Sprite::SpriteShader::DEFALT, true);
    }

    // キャラ選択UI
    {
        auto& charaPick = charaPicksCanvas->AddChildObject();
        charaPick->SetName("CharaPick");
        charaPick->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaPick.ui", Sprite::SpriteShader::GLITCH, true);
    }

    // 時間制限
    {
        auto& timeLimit = charaPicksCanvas->AddChildObject();
        timeLimit->SetName("TimeLimit");
    }
}

// キャラピック更新処理
void CharaPicks::CharaPicksUpdate(float elapsedTime)
{
    // 各キャラ詳細
    CharaDetails();

    // 決定処理
    if (GameObjectManager::Instance().Find("CharaPicksCanvas") != nullptr)
    {
        DecisionButton();
    }
}

// キャラ詳細
void CharaPicks::CharaDetails()
{
    // 決定していたら操作不可
    if (decisionFlg) return;

    GamePad& gamePad = Input::Instance().GetGamePad();
    auto& canvas = GameObjectManager::Instance().Find("CharaPicksCanvas");

    // キャラクター情報を取得する関数
    auto getCharacterInfo = [&](const std::string& charaName, const std::string& objectName, int id) -> CharacterInfo
        {
            auto& chara = canvas->GetChildFind(charaName.c_str());
            auto& charamodel = canvas->GetChildFind(objectName.c_str());
            return
            {
                chara,
                chara->GetComponent<Sprite>(),
                chara->GetChildFind("name"),
                charamodel,
                id
            };
        };

    // キャラ、ID設定
    std::vector<CharacterInfo> characters =
    {
        getCharacterInfo("Kanizo-", "Kanizo-Player", 0),
        getCharacterInfo("Faraic", "FaraicPlayer", 1),
        getCharacterInfo("Santoratto", "SantorattoPlayer", 2),
        getCharacterInfo("Matya-", "Matya-Player", 3)
    };

    // 共通のスプライト初期化処理
    auto initializeSprite = [](std::shared_ptr<Sprite> sprite, const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale, const DirectX::XMFLOAT4& color)
        {
            if (!sprite->IsPlayEasing())
            {
                sprite->spc.position = position;
                sprite->spc.scale = scale;
                sprite->spc.color = color;
            }
        };

    // 選択解除処理
    auto resetCharacter = [](CharacterInfo& chara, const DirectX::XMFLOAT4& defaultColor, const DirectX::XMFLOAT2& defaultScale)
        {
            chara.name->SetEnabled(false);
            chara.charamodel->GetComponent<RendererCom>()->SetEnabled(false);
            chara.name->GetComponent<Sprite>()->StopEasing();
            chara.name->GetComponent<Sprite>()->spc.onshot = false;
            chara.name->GetChildFind("Rightskillicon")->GetComponent<Sprite>()->StopEasing();
            chara.name->GetChildFind("Rightskillicon")->GetComponent<Sprite>()->spc.onshot = false;
            chara.name->GetChildFind("Eskillicon")->GetComponent<Sprite>()->StopEasing();
            chara.name->GetChildFind("Eskillicon")->GetComponent<Sprite>()->spc.onshot = false;
            chara.name->GetChildFind("ultIcon")->GetComponent<Sprite>()->StopEasing();
            chara.name->GetChildFind("ultIcon")->GetComponent<Sprite>()->spc.onshot = false;

            chara.sprite->StopEasing();
            chara.sprite->spc.color = defaultColor;
            chara.sprite->spc.scale = defaultScale;
        };

    // キャラ選択処理
    auto handleCharacterSelection = [&](CharacterInfo& selected, std::vector<CharacterInfo>& others)
        {
            // 初期化
            initializeSprite(selected.name->GetComponent<Sprite>(), { 2200.0f, 200.0f }, { 0.9f, 0.9f }, { 1.0f, 1.0f, 1.0f, 0.0f });
            initializeSprite(selected.name->GetChildFind("Rightskillicon")->GetComponent<Sprite>(), { 2200.0f, 50.0f }, {}, {});
            initializeSprite(selected.name->GetChildFind("Eskillicon")->GetComponent<Sprite>(), { 2200.0f, 190.0f }, {}, {});
            initializeSprite(selected.name->GetChildFind("ultIcon")->GetComponent<Sprite>(), { 2200.0f, 250.0f }, {}, {});

            // チームが既に選んでいるキャラだった場合
            if (selected.id == teamPick)
            {
                resetCharacter(selected, { 1, 0, 0, 1 }, { 0.4f, 0.4f });
                if (selectedCharacterId == selected.id)
                {
                    selectedCharacterId = -1;
                }
                return;
            }

            // キャラ選択処理
            if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown() && selected.sprite->GetHitSprite())
            {
                selected.name->SetEnabled(true);
                selected.charamodel->GetComponent<RendererCom>()->SetEnabled(true);
                selected.name->GetComponent<Sprite>()->EasingPlay();
                selected.name->GetComponent<Sprite>()->spc.onshot = true;
                selected.name->GetChildFind("Rightskillicon")->GetComponent<Sprite>()->EasingPlay();
                selected.name->GetChildFind("Rightskillicon")->GetComponent<Sprite>()->spc.onshot = true;
                selected.name->GetChildFind("Eskillicon")->GetComponent<Sprite>()->EasingPlay();
                selected.name->GetChildFind("Eskillicon")->GetComponent<Sprite>()->spc.onshot = true;
                selected.name->GetChildFind("ultIcon")->GetComponent<Sprite>()->EasingPlay();
                selected.name->GetChildFind("ultIcon")->GetComponent<Sprite>()->spc.onshot = true;

                selected.sprite->spc.color = selectColor;
                selected.sprite->spc.scale = { 0.4f, 0.4f };
                selected.sprite->EasingPlay();

                selectedCharacterId = selected.id;

                // 他のキャラクターをリセット
                for (auto& other : others)
                {
                    resetCharacter(other, color, { 0.4f, 0.4f });
                }
            }
        };

    // 各キャラクターを処理
    for (size_t i = 0; i < characters.size(); ++i)
    {
        std::vector<CharacterInfo> others = characters;
        others.erase(others.begin() + i);
        handleCharacterSelection(characters[i], others);
    }
}

// 決定処理
void CharaPicks::DecisionButton()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    auto& canvas = GameObjectManager::Instance().Find("CharaPicksCanvas");
    auto& decisionButton = canvas->GetChildFind("decision");
    auto& sprite = decisionButton->GetComponent<Sprite>();

    // OKキーの演出
    if (selectedCharacterId != -1 && sprite->GetHitSpriteEnter())
    {
        sprite->EasingPlay();
    }
    else if (!sprite->GetHitSprite())
    {
        sprite->StopEasing();
        sprite->spc.color = color;
    }

    // OKボタンが押され、かつキャラが選択されている場合のみ処理を実行
    if (selectedCharacterId != -1 && GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown() && sprite->GetHitSprite())
    {
        decisionFlg = true;
        GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("charapickcamera"));
    }
}

//キャラのUI追加関数
void CharaPicks::AddCharacterUI(std::shared_ptr<GameObject> parent, const char* charaName, const char* iconPath, const char* namePath, const char* skillIconPath1, const char* skillIconPath2, const char* ultIconPath, const char* modelPath, const char* video1, const char* video2, const char* video3, const char* videoname1/*, const char* videoname2, const char* ultvideoname*/)
{
    // キャラアイコン
    auto& chara = parent->AddChildObject();
    chara->SetName(charaName);
    chara->AddComponent<Sprite>(iconPath, Sprite::SpriteShader::DEFALT, true);

    // 名前表記
    auto& name = chara->AddChildObject();
    name->SetName("name");
    auto& namesprite = name->AddComponent<Sprite>(namePath, Sprite::SpriteShader::DEFALT, false);
    namesprite->SetOrderinLayer(0);
    name->SetEnabled(false);

    //1番目のスキルアイコン
    auto& skillIcon1 = name->AddChildObject();
    skillIcon1->SetName("Rightskillicon");
    auto& skillsprite1 = skillIcon1->AddComponent<Sprite>(skillIconPath1, Sprite::SpriteShader::DEFALT, true);
    skillsprite1->SetOrderinLayer(1);

    //2番目のスキルアイコン
    auto& skillIcon2 = name->AddChildObject();
    skillIcon2->SetName("Eskillicon");
    auto& skillsprite2 = skillIcon2->AddComponent<Sprite>(skillIconPath2, Sprite::SpriteShader::DEFALT, true);
    skillsprite2->SetOrderinLayer(2);

    //ultのスキルアイコン
    auto& ultIcon = name->AddChildObject();
    ultIcon->SetName("ultIcon");
    auto& ultsprite2 = ultIcon->AddComponent<Sprite>(ultIconPath, Sprite::SpriteShader::DEFALT, true);
    ultsprite2->SetOrderinLayer(3);

    //video1
    auto& skillvideo1 = parent->AddChildObject();
    skillvideo1->SetName(videoname1);
    skillvideo1->AddComponent<Video>(video1);
    skillvideo1->transform_->SetWorldPosition({ -2.730f,0.724f,1.765f });
    skillvideo1->transform_->SetEulerRotation({ 0.0f,-32.0f,0.0f });
    skillvideo1->transform_->SetScale({ 1.6f,1.6f,1.0f });
    skillvideo1->SetEnabled(false);

    //video2
    auto& skillvideo2 = parent->AddChildObject();
    skillvideo2->SetName("videoname2");
    skillvideo2->AddComponent<Video>(video2);
    skillvideo2->transform_->SetWorldPosition({ -2.730f,0.724f,1.765f });
    skillvideo2->transform_->SetEulerRotation({ 0.0f,-32.0f,0.0f });
    skillvideo2->transform_->SetScale({ 1.6f,1.6f,1.0f });
    skillvideo2->SetEnabled(false);

    //ult
    auto& ultvideo = parent->AddChildObject();
    ultvideo->SetName("ultvideoname");
    ultvideo->AddComponent<Video>(video3);
    ultvideo->transform_->SetWorldPosition({ -2.730f,0.724f,1.765f });
    ultvideo->transform_->SetEulerRotation({ 0.0f,-32.0f,0.0f });
    ultvideo->transform_->SetScale({ 1.6f,1.6f,1.0f });
    ultvideo->SetEnabled(false);

    // 3Dオブジェクト生成
    auto& obj = parent->AddChildObject();
    std::string playerName = std::string(charaName) + "Player";
    obj->SetName(playerName.c_str());
    obj->transform_->SetWorldPosition({ -0.191f, 0.018f, 1.802f });
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    obj->transform_->SetEulerRotation({ 0.0f, 209.99f, 0.0f });
    auto& r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel(modelPath);
    auto& anim = obj->AddComponent<AnimationCom>();
    anim->PlayAnimation(5, true);
    r->SetEnabled(false);
}

// キャラピック表示設定
void CharaPicks::SetViewCharaPicks(bool flg)
{
    auto& charaPicksCanvas = GameObjectManager::Instance().Find("CharaPicksCanvas");
    if (!charaPicksCanvas) return;

    if (flg)
    {
        charaPicksCanvas->SetEnabled(true);
        GameObjectManager::Instance().Find("lobbyBackParent")->SetEnabled(false);
    }
    else
        charaPicksCanvas->SetEnabled(false);
}