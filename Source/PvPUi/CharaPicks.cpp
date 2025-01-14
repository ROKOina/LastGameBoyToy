#include "CharaPicks.h"
#include "Input/Input.h"
#include "Component/Sprite/Sprite.h"
#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Animation\AnimationCom.h"
#include <Component\Camera\FreeCameraCom.h>

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

    // 各キャラのUI設定
    {
        // KANIZO-
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("Kanizo-");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon0.ui", Sprite::SpriteShader::DEFALT, true);

            // 名前表記
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName0.ui", Sprite::SpriteShader::DEFALT, false);
                name->SetEnabled(false);
            }

            //3Dobject生成
            {
                std::shared_ptr<GameObject> obj = charaPicksCanvas->AddChildObject();
                obj->SetName("Kanizo-Player");
                obj->transform_->SetWorldPosition({ -0.191, 0.018, 1.802 });
                obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
                obj->transform_->SetEulerRotation({ 0.0f,209.99f,0.0f });
                std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
                r->LoadModel("Data/Model/player_True/player1.mdl");
                r->SetEnabled(false);
                std::shared_ptr<AnimationCom>anim = obj->AddComponent<AnimationCom>();
                anim->PlayAnimation(5, true);
            }
        }
        // FARAIC
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("Faraic");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon1.ui", Sprite::SpriteShader::DEFALT, true);

            // 名前表記
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName1.ui", Sprite::SpriteShader::DEFALT, false);
                name->SetEnabled(false);
            }

            //3Dobject生成
            {
                std::shared_ptr<GameObject> obj = charaPicksCanvas->AddChildObject();
                obj->SetName("FaraicPlayer");
                obj->transform_->SetWorldPosition({ -0.191, 0.018, 1.802 });
                obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
                obj->transform_->SetEulerRotation({ 0.0f,209.99f,0.0f });
                std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
                r->LoadModel("Data/Model/player_True/player2.mdl");
                std::shared_ptr<AnimationCom>anim = obj->AddComponent<AnimationCom>();
                anim->PlayAnimation(5, true);
                r->SetEnabled(false);
            }
        }
        // SANTORATTO
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("Santoratto");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon2.ui", Sprite::SpriteShader::DEFALT, true);

            // 名前表記
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName2.ui", Sprite::SpriteShader::DEFALT, false);
                name->SetEnabled(false);
            }

            //3Dobject生成
            {
                std::shared_ptr<GameObject> obj = charaPicksCanvas->AddChildObject();
                obj->SetName("SantorattoPlayer");
                obj->transform_->SetWorldPosition({ -0.191, 0.018, 1.802 });
                obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
                obj->transform_->SetEulerRotation({ 0.0f,209.99f,0.0f });
                std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
                r->LoadModel("Data/Model/player_True/player3.mdl");
                std::shared_ptr<AnimationCom>anim = obj->AddComponent<AnimationCom>();
                anim->PlayAnimation(5, true);
                r->SetEnabled(false);
            }
        }
        // MATYA-
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("Matya-");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon3.ui", Sprite::SpriteShader::DEFALT, true);

            // 名前表記
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName3.ui", Sprite::SpriteShader::DEFALT, false);
                name->SetEnabled(false);
            }

            //3Dobject生成
            {
                std::shared_ptr<GameObject> obj = charaPicksCanvas->AddChildObject();
                obj->SetName("Matya-Player");
                obj->transform_->SetWorldPosition({ -0.191, 0.018, 1.802 });
                obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
                obj->transform_->SetEulerRotation({ 0.0f,209.99f,0.0f });
                std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
                r->LoadModel("Data/Model/player_True/player4.mdl");
                std::shared_ptr<AnimationCom>anim = obj->AddComponent<AnimationCom>();
                anim->PlayAnimation(5, true);
                r->SetEnabled(false);
            }
        }
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

    // 時間制限システム
    TimeLimitSystem(elapsedTime);

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

    auto getCharacterInfo = [&](const std::string& charaName, const std::string& objectname, int id) -> CharacterInfo {
        auto& chara = canvas->GetChildFind(charaName.c_str());
        auto& charamodel = canvas->GetChildFind(objectname.c_str());
        return {
            chara,
            chara->GetComponent<Sprite>(),
            chara->GetChildFind("name"),
            charamodel,
            id
        };
        };

    // キャラ、ID設定
    std::vector<CharacterInfo> characters = {
        getCharacterInfo("Kanizo-","Kanizo-Player", 0),
        getCharacterInfo("Faraic", "FaraicPlayer",1),
        getCharacterInfo("Santoratto","SantorattoPlayer", 2),
        getCharacterInfo("Matya-", "Matya-Player",3)
    };

    // クリックするとスキル表示、キャラ名、選択キャラ、アイコンが表示
    auto handleCharacterSelection = [&](CharacterInfo& selected, std::vector<CharacterInfo>& others) {
        // Spriteクラスで関数を作成（下記は無駄なコード）
        if (!selected.name->GetComponent<Sprite>()->IsPlayEasing())
        {
            selected.name->GetComponent<Sprite>()->spc.position = { 2200.0f, 200.0f };
            selected.name->GetComponent<Sprite>()->spc.scale = { 0.9f, 0.9f };
            selected.name->GetComponent<Sprite>()->spc.color = { 1.0f, 1.0f, 1.0f, 0.0f };
        }

        //チームが選んでいるキャラだった場合はリターン
        if (selected.id == teamPick)
        {
            selected.name->SetEnabled(false);
            selected.charamodel->GetComponent<RendererCom>()->SetEnabled(false);
            selected.name->GetComponent<Sprite>()->StopEasing();
            selected.name->GetComponent<Sprite>()->spc.onshot = false;

            selected.sprite->StopEasing();

            selected.sprite->spc.color = { 1,0,0,1 };
            if (selectedCharacterId == selected.id)
            {
                selectedCharacterId = -1;
            }
            return;
        }

        if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown() && selected.sprite->GetHitSprite()) {
            selected.name->SetEnabled(true);
            selected.charamodel->GetComponent<RendererCom>()->SetEnabled(true);
            selected.name->GetComponent<Sprite>()->EasingPlay();
            selected.name->GetComponent<Sprite>()->spc.onshot = true;

            selected.sprite->spc.color = selectColor;
            selected.sprite->spc.scale = { 1.0f, 1.0f };
            selected.sprite->EasingPlay();

            selectedCharacterId = selected.id;

            for (auto& other : others) {
                if (&other != &selected) {
                    other.name->SetEnabled(false);
                    other.charamodel->GetComponent<RendererCom>()->SetEnabled(false);
                    other.name->GetComponent<Sprite>()->StopEasing();
                    other.name->GetComponent<Sprite>()->spc.onshot = false;

                    other.sprite->StopEasing();
                    other.sprite->spc.color = color;
                    other.sprite->spc.scale = { 1.0f, 1.0f };
                }
            }
        }
        };

    for (size_t i = 0; i < characters.size(); ++i) {
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

// 時間制限システム
void CharaPicks::TimeLimitSystem(float elapsedTime)
{
    // 制限時間は設定できるように
    // 制限時間あり、なし切り替えれるようにする
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