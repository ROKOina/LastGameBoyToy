#include "ScenePVP.h"
#include <Graphics\Graphics.h>
#include <Component\Camera\FreeCameraCom.h>
#include <Component\Camera\EventCameraCom.h>
#include <Component\Character\RegisterChara.h>
#include <Component\Collsion\ColliderCom.h>
#include <Component\Camera\EventCameraManager.h>
#include <Input\Input.h>
#include <Component\Character\CharacterCom.h>
#include <Component\Animation\AnimationCom.h>
#include <Component\System\SpawnCom.h>
#include <Component\Stage\StageEditorCom.h>
#include "Component\Phsix\RigidBodyCom.h"
#include <Component\Collsion\RayCollisionCom.h>
#include <Component\MoveSystem\EasingMoveCom.h>
#include "Component\Collsion\PushBackCom.h"
#include "Component\Enemy\BossCom.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\Light\LightCom.h"
#include <Component/System/HitProcessCom.h>
#include "Component/Particle/CPUParticle.h"
#include "Component/Particle/GPUParticle.h"
#include <Component/UI/UiSystem.h>
#include <Component/UI/PlayerUI.h>
#include <Component/UI/UiFlag.h>
#include <Component/UI/UiGauge.h>
#include "Netwark/Photon/StdIO_UIListener.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include "Component\Stage\GateGimmickCom.h"
#include <StateMachine\Behaviar\InazawaCharacterState.h>
#include "Component\Sprite\Sprite.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Component\UI\Font.h"
#include "Math/easing.h"
#include "Component\GameSystem\RespawnCom.h"

#include "Component/Renderer/InstanceRendererCom.h"

#include "Netwark/Photon/Photon_lib.h"
#include "../SceneTitle/SceneTitle.h"

#include "PvPUi/CharaPicks.h"
#include "Setting/Setting.h"

void ScenePVP::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //ポストエフェクト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        obj->AddComponent<PostEffect>();
    }

    //ライト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>("Data/SerializeData/LightData/pvp.light");
    }

    //ロビー選択から始まる
    //InitializePVP();
    InitializeLobbySelect();

    charaPicks = std::make_shared<CharaPicks>();

    // キャラピックUI生成
    charaPicks->CreateCharaPicksUiObject();
    charaPicks->SetViewCharaPicks(false);

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    //ネット大事
    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);

    //ゲームシステム
    pvpGameSystem = std::make_unique<PVPGameSystem>();

    //UI初期化
    PlayerUIManager::Instance().ResetAllyHp();
}

void ScenePVP::InitializeLobbySelect()
{
    //背景
    InitializeBack();

    //font
    std::shared_ptr<GameObject> FParent = GameObjectManager::Instance().Create();
    FParent->SetName("fontParent");
    //削除予定リストに追加
    tempRemoveObj.emplace_back(FParent);

    std::shared_ptr<GameObject> lobbyNameGage = FParent->AddChildObject();
    lobbyNameGage->SetName("lobbyNameGage");
    lobbyNameGage->AddComponent<UiSystem>("Data/SerializeData/UIData/PVPScene/lobbyNameInput.ui", Sprite::SpriteShader::DEFALT, false);
    //削除予定リストに追加
    tempRemoveObj.emplace_back(lobbyNameGage);

    for (auto& lf : lobbySelectFont)
    {
        if (lf.collision)
        {
            std::shared_ptr<GameObject> kariLOBBYSELECT = FParent->AddChildObject();
            //std::shared_ptr<GameObject> kariLOBBYSELECT = GameObjectManager::Instance().Create();
            kariLOBBYSELECT->SetName(("FontBack" + std::to_string(lf.id)).c_str());
            kariLOBBYSELECT->AddComponent<UiSystem>("Data/SerializeData/UIData/PVPScene/lobbyStrBack.ui", Sprite::SpriteShader::DEFALT, true);
            //削除予定リストに追加
            tempRemoveObj.emplace_back(kariLOBBYSELECT);
        }

        std::shared_ptr<GameObject> obj = FParent->AddChildObject();
        //std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName(("lobbySelectFont" + std::to_string(lf.id)).c_str());
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = lf.pos;
        font->str = lf.str;  //L付けてね
        font->scale = lf.scale;
        font->color = lf.color;
        //削除予定リストに追加
        tempRemoveObj.emplace_back(obj);
    }
}

void ScenePVP::InitializeLobby()
{
    //背景
    InitializeBack();

    //font
    std::shared_ptr<GameObject> FParent = GameObjectManager::Instance().Create();
    FParent->SetName("fontParent");
    //削除予定リストに追加
    tempRemoveObj.emplace_back(FParent);

    //master表示画像
    std::shared_ptr<GameObject> masterSpr = FParent->AddChildObject();
    masterSpr->SetName("masterSpr");
    masterSpr->AddComponent<UiSystem>("Data/SerializeData/UIData/PVPScene/lobbyMasterPlayer.ui", Sprite::SpriteShader::DEFALT, false);
    //削除予定リストに追加
    tempRemoveObj.emplace_back(masterSpr);

    //チーム分け画像
    for (int i = 0; i < 4; ++i)
    {
        std::shared_ptr<GameObject> teamColor = FParent->AddChildObject();
        teamColor->SetName(("teamColor" + std::to_string(i)).c_str());
        teamColor->AddComponent<UiSystem>("Data/SerializeData/UIData/PVPScene/lobbyStrBack.ui", Sprite::SpriteShader::DEFALT, true);
        //削除予定リストに追加
        tempRemoveObj.emplace_back(teamColor);
    }

    for (auto& lf : lobbyFont)
    {
        if (lf.collision)
        {
            std::shared_ptr<GameObject> kariLOBBY = FParent->AddChildObject();
            //std::shared_ptr<GameObject> kariLOBBYSELECT = GameObjectManager::Instance().Create();
            kariLOBBY->SetName(("FontBack" + std::to_string(lf.id)).c_str());
            kariLOBBY->AddComponent<UiSystem>("Data/SerializeData/UIData/PVPScene/lobbyStrBack.ui", Sprite::SpriteShader::DEFALT, true);
            //削除予定リストに追加
            tempRemoveObj.emplace_back(kariLOBBY);
        }

        std::shared_ptr<GameObject> obj = FParent->AddChildObject();
        obj->SetName(("lobbyFont" + std::to_string(lf.id)).c_str());
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = lf.pos;
        font->str = lf.str;  //L付けてね
        font->scale = lf.scale;
        font->color = lf.color;
        //削除予定リストに追加
        tempRemoveObj.emplace_back(obj);
    }
}

void ScenePVP::InitializeCharaSelect()
{
    //背景
    InitializeBack();    //ピック画面起動

    charaPicks->SetViewCharaPicks(true);
}

void ScenePVP::InitializePVP()
{
#pragma region ゲームオブジェクトの設定

#ifdef _DEBUG
    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -103 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();
#endif

    //ステージ
    {
        auto& stageObj = GameObjectManager::Instance().Create();
        stageObj->SetName("stage");
        stageObj->transform_->SetWorldPosition({ 0, 0, 0 });

        float size = 0.05f;
        stageObj->transform_->SetScale({ size, size, size });
        std::shared_ptr<RendererCom> r = stageObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/AbeStage/AbeStage_light.mdl");
        stageObj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");

        //ステージ
        StageEditorCom* stageEdit = stageObj->AddComponent<StageEditorCom>().get();
        //判定生成
        stageEdit->PlaceStageRigidCollider("Data/Model/AbeStage/", "AbeStage_light.mdl", "__", size);
        //Jsonからオブジェクト配置
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/AbeStage_Spawn.json");

        //リスポーン用
        GameObj respawnObj = GameObjectManager::Instance().Create();
        respawnObj->SetName("respawn");
        RespawnCom* spawnCom = respawnObj->AddComponent<RespawnCom>().get();
        spawnCom->SetGameMode(pvpGameSystem->GetGameMode());

        //スポーン位置設定
        StageEditorCom::PlaceObject spawnObj = stageEdit->GetPlaceObject("Spawn");
        for (auto& res : spawnObj.objList)
        {
            spawnCom->AddRespawnPoses(res->transform_->GetWorldPosition());
        }

        //プレイヤー
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST(charaPicks->GetSelectedCharacterId()), obj, true);

        int id = obj->GetComponent<CharacterCom>()->GetNetCharaData().GetNetPlayerID();
        obj->transform_->SetWorldPosition(spawnCom->GetRespawnPoses()[id]);
    }

    //イベント用カメラ
    {
        std::shared_ptr<GameObject> eventCamera = GameObjectManager::Instance().Create();
        eventCamera->SetName("eventcamera");
        eventCamera->AddComponent<EventCameraCom>();
        eventCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //snowparticle
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("snowparticle");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/snow.gpuparticle", 10000);
    }

    //UI
    std::shared_ptr<GameObject> gameModeUI = GameObjectManager::Instance().Create();
    gameModeUI->SetName("gameModeUI");

    switch (pvpGameSystem->GetGameMode())
    {
    case PVPGameSystem::GAME_MODE::Deathmatch:
    {
        std::shared_ptr<GameObject> obj = gameModeUI->AddChildObject();
        obj->SetName("time");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 900,0 };
        font->scale = 1.5f;
        font->color = { 1,1,1,1 };
    }
    {
        std::shared_ptr<GameObject> obj = gameModeUI->AddChildObject();
        obj->SetName("killCountAlly");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 1560,23 };
        font->scale = 1.0f;
        font->color = { 0,0,1,1 };
    }
    {
        std::shared_ptr<GameObject> obj = gameModeUI->AddChildObject();
        obj->SetName("killCountEnemy");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 1560,119 };
        font->scale = 1.0f;
        font->color = { 1,0,0,1 };
    }
    break;
    }

    //キャラピックオブジェクトを消去
    GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("CharaPicksCanvas"));

#pragma endregion
}

static const int rectNum = 10;
static const int crossNum = 10;
static const int triangleNum = 3;
void ScenePVP::InitializeBack()
{
    //背景
    std::shared_ptr<GameObject> lobbyBackParent = GameObjectManager::Instance().Create();
    lobbyBackParent->SetName("lobbyBackParent");
    tempRemoveObj.emplace_back(lobbyBackParent);

    std::shared_ptr<GameObject> lobbyBack = lobbyBackParent->AddChildObject();
    lobbyBack->SetName("lobbyBack");
    auto& uiSys = lobbyBack->AddComponent<UiSystem>("Data/SerializeData/UIData/PVPScene/lobbyBack.ui", Sprite::SpriteShader::DEFALT, false);
    uiSys->SetOrderinLayer(-10);
    //削除予定リストに追加
    tempRemoveObj.emplace_back(lobbyBack);
    //背景移動オブジェクト
    for (int r = 0; r < rectNum; ++r) //四角
    {
        std::shared_ptr<GameObject> lobbyBackRectSmall = lobbyBackParent->AddChildObject();
        lobbyBackRectSmall->SetName(std::string("lobbyBackRectSmall" + std::to_string(r)).c_str());
        auto& rss = lobbyBackRectSmall->AddComponent<UiSystem>("Data/SerializeData/UIData/PVPScene/lobbyBackRect.ui", Sprite::SpriteShader::DEFALT, false);
        rss->EasingPlay();
        rss->SetOrderinLayer(-5);
        //削除予定リストに追加
        tempRemoveObj.emplace_back(lobbyBackRectSmall);
        std::shared_ptr<GameObject> lobbyBackRectBig = lobbyBackParent->AddChildObject();
        lobbyBackRectBig->SetName(std::string("lobbyBackRectBig" + std::to_string(r)).c_str());
        auto& rbs = lobbyBackRectBig->AddComponent<UiSystem>("Data/SerializeData/UIData/PVPScene/lobbyBackRect.ui", Sprite::SpriteShader::DEFALT, false);
        rbs->EasingPlay();
        rbs->SetOrderinLayer(-5);
        //削除予定リストに追加
        tempRemoveObj.emplace_back(lobbyBackRectBig);
    }

    for (int c = 0; c < 2; ++c) //円
    {
        std::shared_ptr<GameObject> lobbyBackCircle = lobbyBackParent->AddChildObject();
        lobbyBackCircle->SetName(std::string("lobbyBackCircle" + std::to_string(c)).c_str());
        auto& uiC = lobbyBackCircle->AddComponent<UiSystem>(std::string("Data/SerializeData/UIData/PVPScene/lobbyBackCircle0" + std::to_string(c + 1) + ".ui").c_str(), Sprite::SpriteShader::DEFALT, false);
        uiC->SetOrderinLayer(-5);
        //削除予定リストに追加
        tempRemoveObj.emplace_back(lobbyBackCircle);
    }

    for (int b = 0; b < crossNum; ++b) //バツ
    {
        std::shared_ptr<GameObject> lobbyBackCross = lobbyBackParent->AddChildObject();
        lobbyBackCross->SetName(std::string("lobbyBackCross" + std::to_string(b)).c_str());
        auto& cs = lobbyBackCross->AddComponent<UiSystem>("Data/SerializeData/UIData/PVPScene/lobbyBackCross.ui", Sprite::SpriteShader::DEFALT, false);
        cs->EasingPlay();
        cs->SetOrderinLayer(-5);
        //削除予定リストに追加
        tempRemoveObj.emplace_back(lobbyBackCross);
    }
}

void ScenePVP::Finalize()
{
    photonNet->close();
}

void TransitionRemove(std::vector<std::weak_ptr<GameObject>>& removeObjs)
{
    for (auto& removeObj : removeObjs)
    {
        //保存オブジェクトすべて削除
        GameObjectManager::Instance().Remove(removeObj.lock());
    }
    removeObjs.clear();
}

void ScenePVP::Update(float elapsedTime)
{
    //ネット更新
    photonNet->run(elapsedTime);

    //イベントカメラ用
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    //Ui更新
    PlayerUIManager::Instance().UIUpdate(elapsedTime);

    //画面に応じた更新処理
    TransitionUpdate(elapsedTime);

    //ゲームシステム更新
    GameSystemUpdate(elapsedTime);
    pvpGameSystem->update(elapsedTime);

    //終わり
    if (pvpGameSystem->IsGameEnd())
    {
        //仮遷移
        if (!SceneManager::Instance().GetTransitionFlag())
            SceneManager::Instance().ChangeSceneDelay(new SceneTitle, 2);
    }

    //画面切り替え処理
    auto net = photonNet->GetPhotonLib();
    bool lobbyOneFlg = false;   //一回だけ初期化するように(ロビーに)
    if (!isLobby) //一回だけ入る
    {
        if (net->IsJoinPermission() || net->GetIsMasterPlayer())    //ネットに繋がった時ロビーに入る
        {
            lobbyOneFlg = true;
            isLobby = true;
            TransitionRemove(tempRemoveObj);
            lobbyState++;   //1
            fontState = 0;
        }
    }
    bool lobbySelectOneFlg = false;   //一回だけ初期化するように(キャラセレクトに)
    if (!isCharaSelect) //一回だけ入る
    {
        if (net->GetIsCharaSelect())    //キャラセレクト
        {
            lobbySelectOneFlg = true;
            isCharaSelect = true;
            TransitionRemove(tempRemoveObj);
            lobbyState++;   //2
            fontState = 0;
        }
    }
    bool GameOneFlg = false;   //一回だけ初期化するように(ゲーム開始に)
    if (!isGame) //一回だけ入る
    {
        if (net->GetIsGamePlay())    //ゲーム開始
        {
            GameOneFlg = true;
            isGame = true;
            TransitionRemove(tempRemoveObj);
            lobbyState++;   //3
            fontState = 0;

            //ゲームモード設定
            pvpGameSystem->SetGameMode(PVPGameSystem::GAME_MODE(net->GetGameMode()));
        }
    }

    //ゲームオブジェクトの行列更新
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);

    if (lobbyOneFlg)    //一回だけ入る
    {
        InitializeLobby();
    }
    if (lobbySelectOneFlg)    //一回だけ入る
    {
        InitializeCharaSelect();
    }
    if (GameOneFlg)    //一回だけ入る
    {
        InitializePVP();
    }

    //設定からキャラピックを選んだ場合
    auto& ss = SceneManager::Instance().GetSettingScreen();
    if (ss->GetIsCharaSelect())
    {
        ss->SetViewSetting(false);
        charaPicks->SetViewCharaPicks(true);
    }

    // キャラピック更新処理
    charaPicks->CharaPicksUpdate(elapsedTime);
}

void ScenePVP::Render(float elapsedTime)
{
    // 画面クリア＆レンダーターゲット設定
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //コンスタントバッファの更新
    ConstantBufferUpdate(elapsedTime);

    //サンプラーステートの設定
    Graphics::Instance().SetSamplerState();

    //オブジェクト生成関数
#ifdef _DEBUG
    NewObject();
#endif

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());

    //imgui
    photonNet->ImGui();

    //イベントカメラ用
    EventCameraManager::Instance().EventCameraImGui();
}

//オブジェクト生成関数
void ScenePVP::NewObject()
{
    ImGui::Begin("CreateObject");

    if (ImGui::Button("gpuparticle"))
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testgpuparticle");
        obj->AddComponent<GPUParticle>(nullptr, 10000);
    }
    ImGui::SameLine();
    if (ImGui::Button("cpuparticle"))
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testcpuparticle");
        obj->AddComponent<CPUParticle>(nullptr, 10000);
    }
    ImGui::SameLine();
    if (ImGui::Button("ui"))
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testui");
        obj->AddComponent<Sprite>(nullptr, Sprite::SpriteShader::DEFALT, true);
    }
    if (ImGui::Button("light"))
    {
        std::shared_ptr<GameObject>obj = GameObjectManager::Instance().Create();
        obj->SetName("testlight");
        obj->AddComponent<Light>(nullptr);
    }

    ImGui::End();
}

void ScenePVP::GameSystemUpdate(float elapsedTime)
{
    auto net = photonNet->GetPhotonLib();
    //各ゲームモード必要情報更新
    switch (pvpGameSystem->GetGameMode())
    {
    case PVPGameSystem::GAME_MODE::Deathmatch:
    {
        auto& DM = pvpGameSystem->GetDeathMatchData();
        DM.teamData[PVPGameSystem::TEAM_KIND::RED_GROUP].killCount = net->GetKillCount(PVPGameSystem::TEAM_KIND::RED_GROUP);
        DM.teamData[PVPGameSystem::TEAM_KIND::BLUE_GROUP].killCount = net->GetKillCount(PVPGameSystem::TEAM_KIND::BLUE_GROUP);
        DM.nowTime = net->GetNowTime();
    }
    break;
    case PVPGameSystem::GAME_MODE::Crown:

        break;
    case PVPGameSystem::GAME_MODE::Button:

        break;
    }
}

// UTF-8 (std::string) → UTF-16 (std::wstring) 変換
std::wstring UTF8ToWString2(const std::string& str) {
    std::wstring result;
    size_t i = 0;
    while (i < str.size()) {
        unsigned char c = str[i];
        if (c <= 0x7F) {
            result.push_back(c);
            ++i;
        }
        else if ((c & 0xE0) == 0xC0) {
            wchar_t wc = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
            result.push_back(wc);
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0) {
            wchar_t wc = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
            result.push_back(wc);
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0) {
            wchar_t wc = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) | ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
            result.push_back(wc);
            i += 4;
        }
        else {
            // 不正なUTF-8データを無視する
            ++i;
        }
    }
    return result;
}

void ScenePVP::TransitionUpdate(float elapsedTime)
{
    switch (lobbyState)
    {
    case 0: //ロビー選択
        LobbySelectFontUpdate(elapsedTime); //font
        LobbyBackSprUpdate(elapsedTime);    //背景
        break;
    case 1: //ロビー
        LobbyFontUpdate(elapsedTime);   //font
        LobbyBackSprUpdate(elapsedTime);    //背景
        break;
    case 2: //キャラ選択
        CharaSelectUpdate(elapsedTime);
        LobbyBackSprUpdate(elapsedTime);    //背景
        break;
    case 3: //ゲーム中
        GameUpdate(elapsedTime);

        break;
    }
}

void ScenePVP::LobbySelectFontUpdate(float elapsedTime)
{
    //font当たり背景セット
    auto& fP = GameObjectManager::Instance().Find("fontParent");
    for (auto& f : lobbySelectFont)
    {
        auto& back = fP->GetChildFind(("FontBack" + std::to_string(f.id)).c_str()); //背景オブジェクト
        auto& fontObj = fP->GetChildFind(("lobbySelectFont" + std::to_string(f.id)).c_str());  //文字

        //下で処理するためここではfalseに
        if (f.id >= 20)
        {
            if (back)
                back->SetEnabled(false);
            if (fontObj)
                fontObj->SetEnabled(false);
            continue;
        }

        //ステートで有効判定
        if (f.state != fontState)
        {
            if (back)
                back->SetEnabled(false);
            if (fontObj)
                fontObj->SetEnabled(false);
            continue;
        }
        if (back)
            back->SetEnabled(true);
        if (fontObj)
            fontObj->SetEnabled(true);

        //判定
        if (f.collision)
        {
            auto& font = fontObj->GetComponent<Font>();
            auto& ui = back->GetComponent<UiSystem>();

            //各情報更新
            back->transform_->SetWorldPosition({ font->position.x,font->position.y ,0 });
            ui->spc.scale.x = 0.096f * font->str.length() * font->scale;
            ui->spc.scale.y = 0.096f * font->scale;

            //判定
            if (ui->GetHitSprite())
            {
                ui->spc.color.x = 1;

                GamePad& gamePad = Input::Instance().GetGamePad();
                if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
                {
                    auto& lobbyStr = fP->GetChildFind(("lobbySelectFont" + std::to_string(2)).c_str());  //文字
                    auto& lobbyF = lobbyStr->GetComponent<Font>();

                    if (f.id == 4 || f.id == 11)    //戻る
                        fontState = 0;
                    else if (f.id == 0)     //新規
                    {
                        fontState = 1;
                        lobbyF->str = L"";
                    }
                    else if (f.id == 10)    //参加
                    {
                        fontState = 2;
                        joinRoomCount = -1;
                        lobbyF->str = L"";
                    }
                    else if (f.id == 3) //ルーム作成
                    {
                        if (lobbyF->str.length() > 0)
                        {
                            auto net = photonNet->GetPhotonLib();
                            net->SetRoomName(lobbyF->str);
                            net->StartConnect();
                        }

                        break;
                    }
                    else if (f.id == 12)    //入室
                    {
                        if (lobbyF->str.length() > 0)
                        {
                            auto net = photonNet->GetPhotonLib();
                            net->SetRoomName(lobbyF->str);
                            net->StartConnect();
                        }

                        break;
                    }
                    //ヒット情報リセット
                    for (auto& f : lobbySelectFont)
                    {
                        auto& backf = fP->GetChildFind(("FontBack" + std::to_string(f.id)).c_str()); //背景オブジェクト
                        if (!backf)continue;
                        auto& backUi = backf->GetComponent<UiSystem>();
                        backUi->SetHitSprite(false);
                    }
                }
            }
            else
                ui->spc.color.x = 0;
        }

        //ロビー名入力
        auto& gage = fP->GetChildFind("lobbyNameGage"); //背景オブジェクト
        gage->SetEnabled(false);
        if (fontState == 1)
        {
            gage->SetEnabled(true);

            if (f.id == 2)  //入力用
            {
                auto& font = fontObj->GetComponent<Font>();
                //一文字削除
                static bool BSflg = false;  //連続対処
                bool BSOneFlg = false;
                if (GetKeyState(8) & 0x8000)
                {
                    BSOneFlg = true;
                    if (!BSflg)
                    {
                        if (font->str.length() > 0)
                            font->str.erase(font->str.end() - 1);
                    }
                }
                BSflg = BSOneFlg;

                //文字入力
                static bool inputFlg[26];  //連続対処
                for (int i = 65; i < 91; ++i)
                {
                    bool oneIn = false;
                    if (GetKeyState(i) & 0x8000)
                    {
                        oneIn = true;
                        if (!inputFlg[91 - i])
                        {
                            char a = char(i);
                            font->str.push_back(static_cast<wchar_t>(a));
                        }
                    }
                    inputFlg[91 - i] = oneIn;
                }
                //文字入力
                static bool numFlg[10];  //連続対処
                for (int i = 48; i < 58; ++i)
                {
                    bool oneIn = false;
                    if (GetKeyState(i) & 0x8000)
                    {
                        oneIn = true;
                        if (!numFlg[58 - i])
                        {
                            char a = char(i);
                            font->str.push_back(static_cast<wchar_t>(a));
                        }
                    }
                    numFlg[58 - i] = oneIn;
                }
            }
        }
    }
    //部屋名
    if (fontState == 2)
    {
        //部屋名を並べる
        auto net = photonNet->GetPhotonLib();
        int count = 20;
        for (auto& room : net->GetRoomNames())
        {
            auto& back = fP->GetChildFind(("FontBack" + std::to_string(count)).c_str()); //背景オブジェクト
            auto& fontObj = fP->GetChildFind(("lobbySelectFont" + std::to_string(count)).c_str());  //文字
            if (back)
                back->SetEnabled(true);
            if (fontObj)
                fontObj->SetEnabled(true);

            auto& font = fontObj->GetComponent<Font>();
            auto& ui = back->GetComponent<UiSystem>();

            //情報更新
            back->transform_->SetWorldPosition({ font->position.x,font->position.y ,0 });
            font->position.y = 218 + 80 * (count - 20);
            font->str = room;

            //判定
            if (ui->GetHitSprite())
            {
                GamePad& gamePad = Input::Instance().GetGamePad();
                if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
                {
                    auto& lobbyStr = fP->GetChildFind(("lobbySelectFont" + std::to_string(2)).c_str());  //文字
                    auto& lobbyF = lobbyStr->GetComponent<Font>();
                    lobbyF->str = room;
                    joinRoomCount = count;

                    break;
                }
                ui->spc.color = { 1,0,0,1 };
            }
            else
                ui->spc.color = { 0,0,1,1 };

            if (joinRoomCount == count)
                ui->spc.color = { 1,0,1,1 };

            count++;
        }
    }
}

void ScenePVP::LobbyFontUpdate(float elapsedTime)
{
    auto& fP = GameObjectManager::Instance().Find("fontParent");
    for (auto& f : lobbyFont)
    {
        auto& back = fP->GetChildFind(("FontBack" + std::to_string(f.id)).c_str()); //背景オブジェクト
        auto& fontObj = fP->GetChildFind(("lobbyFont" + std::to_string(f.id)).c_str());  //文字

        //下で処理するためここではfalseに
        if (f.id >= 20)
        {
            if (back)
                back->SetEnabled(false);
            if (fontObj)
                fontObj->SetEnabled(false);
            continue;
        }

        //ステートで有効判定
        if (f.state != fontState)
        {
            if (back)
                back->SetEnabled(false);
            if (fontObj)
                fontObj->SetEnabled(false);
            continue;
        }
        if (back)
            back->SetEnabled(true);
        if (fontObj)
            fontObj->SetEnabled(true);

        //判定
        if (f.collision)
        {
            auto& font = fontObj->GetComponent<Font>();
            auto& ui = back->GetComponent<UiSystem>();

            //各情報更新
            back->transform_->SetWorldPosition({ font->position.x,font->position.y ,0 });
            ui->spc.scale.x = 0.096f * font->str.length() * font->scale;
            ui->spc.scale.y = 0.096f * font->scale;

            //判定
            if (ui->GetHitSprite())
            {
                ui->spc.color.x = 1;

                GamePad& gamePad = Input::Instance().GetGamePad();
                if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
                {
                    auto net = photonNet->GetPhotonLib();
                    //キャラピック開始
                    if (f.id == 2)
                    {
                        net->SetIsCharaSelect();
                    }

                    //ゲームモード
                    if (f.id == 11) //チームデスマッチ
                    {
                        net->SetGameMode(1);
                    }
                    if (f.id == 12) //王冠
                    {
                        net->SetGameMode(2);
                    }

                    //ヒット情報リセット
                    for (auto& f : lobbySelectFont)
                    {
                        auto& backf = fP->GetChildFind(("FontBack" + std::to_string(f.id)).c_str()); //背景オブジェクト
                        if (!backf)continue;
                        auto& backUi = backf->GetComponent<UiSystem>();
                        backUi->SetHitSprite(false);
                    }
                }
            }
            else
            {
                ui->spc.color.x = 0;
            }
        }
    }

    //決定モードの色を変える
    auto net = photonNet->GetPhotonLib();
    int mode = net->GetGameMode();
    for (int i = 1; i < 4; ++i)
    {
        auto& backDeath = fP->GetChildFind(("FontBack" + std::to_string(10 + i)).c_str()); //背景オブジェクト
        if (!backDeath)break;
        auto& DUI = backDeath->GetComponent<UiSystem>();
        if (mode == i)
            DUI->spc.color = { 0,1,1,1 };
        else
            DUI->spc.color = { 0.4f,0.4f,0.4f,1 };
    }

    //ネットキャラID表示
    if (fontState == 0)
    {
        //ネットキャラを並べる
        auto net = photonNet->GetPhotonLib();
        int count = 19;
        int playerID = -1;
        for (auto& saveInput : net->GetSaveInput())
        {
            count++;
            playerID++;

            //ネットキャラIDとマスタープレイヤー表示
            auto& fontObj = fP->GetChildFind(("lobbyFont" + std::to_string(count)).c_str());  //文字
            if (fontObj)
                fontObj->SetEnabled(false);

            if (!saveInput.useFlg)continue;

            if (fontObj)
                fontObj->SetEnabled(true);

            auto& master = fP->GetChildFind("masterSpr");  //マスタープレイヤー画像
            auto& font = fontObj->GetComponent<Font>();

            //情報更新
            font->position.y = 297 + 80 * (count - 20);
            font->str = UTF8ToWString2(saveInput.name);
            if (net->GetMasterPlayerID() == playerID)   //マスタープレイヤーの場合
            {
                auto Mpos = master->transform_->GetWorldPosition();
                Mpos.y = font->position.y + 23;
                master->transform_->SetWorldPosition(Mpos);
            }
        }

        //チーム分け画像色変更
        for (int playerIDTeam = 0; playerIDTeam < 4; playerIDTeam++)
        {
            auto& teamColorSpr = fP->GetChildFind(("teamColor" + std::to_string(playerIDTeam)).c_str()); //チーム色画像
            auto& teamColorUI = teamColorSpr->GetComponent<UiSystem>();

            //位置
            teamColorSpr->transform_->SetWorldPosition({ 1080.0f,297.0f + 80 * playerIDTeam,0 });
            //UI大きさ
            teamColorUI->spc.scale.x = 0.2f;

            //チーム分け取得
            if (net->GetTeamID(playerIDTeam) == PVPGameSystem::TEAM_KIND::RED_GROUP)
                teamColorUI->spc.color = { 1,0,0,1 };
            else
                teamColorUI->spc.color = { 0,0,1,1 };

            //ホストのみ
            if (net->GetIsMasterPlayer())
            {
                //判定
                if (teamColorUI->GetHitSprite())
                {
                    GamePad& gamePad = Input::Instance().GetGamePad();
                    if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
                    {
                        if (teamColorUI->spc.color.x > 0.5f)
                            teamColorUI->spc.color = { 0,0,1,1 };
                        else
                            teamColorUI->spc.color = { 1,0,0,1 };
                    }
                }

                //ネットに送信
                if (teamColorUI->spc.color.x > 0.5f)
                    net->SetTeamID(PVPGameSystem::TEAM_KIND::RED_GROUP, playerIDTeam);
                else
                    net->SetTeamID(PVPGameSystem::TEAM_KIND::BLUE_GROUP, playerIDTeam);
            }
        }
    }
}

void ScenePVP::CharaSelectUpdate(float elapsedTime)
{
    auto net = photonNet->GetPhotonLib();

    //キャラ被りを無くす
    auto& netSaveInput = net->GetSaveInput();

    //全員ピック確認
    bool pickTransition = true;
    for (auto& s : netSaveInput)
    {
        if (!s.useFlg)continue; //ウーズフラグONなってる？
        if (s.charaID >= 0)
        {
            //味方のキャラが確定ならピックに送る
            if (s.teamID == net->GetTeamID(net->GetMyPlayerID()))
                charaPicks->SetTeamPick(s.charaID);
            continue;
        }

        pickTransition = false;
    }
    if (pickTransition) //ピック完了
    {
        //ゲームスタート
        charaPicks->SetViewCharaPicks(false);
        net->PlayGameStart();
    }

    //ピック確定
    if (charaPicks->IsDecisionFlg())
    {
        net->SetMyPickCharaID(charaPicks->GetSelectedCharacterId());
    }
}

void ScenePVP::GameUpdate(float elapsedTime)
{
    auto net = photonNet->GetPhotonLib();

    //味方UI登録
    auto& saveI = net->GetSaveInput();
    auto& player = GameObjectManager::Instance().Find("player");
    auto& netData = player->GetComponent<CharacterCom>()->GetNetCharaData();
    if (!PlayerUIManager::Instance().GetAllyHp())
    {
        for (auto& s : saveI)
        {
            if (!s.useFlg)continue;
            if (netData.GetTeamID() != s.teamID)continue;

            std::string name = "netPlayer" + std::to_string(s.photonId);
            GameObj netPlayer = GameObjectManager::Instance().Find(name.c_str());
            if (!netPlayer)continue;

            PlayerUIManager::Instance().CreateNetTeamUI(netPlayer);
            break;
        }
    }

    //使用キャラUI更新
    int charaID[4] = { -1,-1,-1,-1 };   //前２個は味方
    for (auto& s : saveI)
    {
        if (!s.useFlg)continue;
        if (netData.GetTeamID() == s.teamID)
            if (charaID[0] < 0)charaID[0] = s.charaID;
            else charaID[1] = s.charaID;
        else
            if (charaID[2] < 0)charaID[2] = s.charaID;
            else charaID[3] = s.charaID;
    }
    PlayerUIManager::Instance().NetUseCharaUIUpdate(charaID);

    //ゲームモードUI更新
    switch (pvpGameSystem->GetGameMode())
    {
    case PVPGameSystem::GAME_MODE::Deathmatch:
    {
        auto& gameModeUI = GameObjectManager::Instance().Find("gameModeUI");
        auto& time = gameModeUI->GetChildFind("time")->GetComponent<Font>();
        auto& killA = gameModeUI->GetChildFind("killCountAlly")->GetComponent<Font>();
        auto& killE = gameModeUI->GetChildFind("killCountEnemy")->GetComponent<Font>();
        auto& des = pvpGameSystem->GetDeathMatchData();
        time->str = UTF8ToWString2(std::to_string(int(des.endTime - des.nowTime)));

        //チームによって変える
        if (netData.GetTeamID() == 0)
        {
            killA->str = UTF8ToWString2(std::to_string(des.teamData[0].killCount));
            killE->str = UTF8ToWString2(std::to_string(des.teamData[1].killCount));
        }
        else
        {
            killA->str = UTF8ToWString2(std::to_string(des.teamData[1].killCount));
            killE->str = UTF8ToWString2(std::to_string(des.teamData[0].killCount));
        }
    }
    break;
    }
}

void ScenePVP::LobbyBackSprUpdate(float elapsedTime)
{
    auto& lobbyBackParent = GameObjectManager::Instance().Find("lobbyBackParent");
    for (int r = 0; r < rectNum; ++r) //四角
    {
        auto& lobbyBackRectSmall = lobbyBackParent->GetChildFind(std::string("lobbyBackRectSmall" + std::to_string(r)).c_str());
        auto& lobbyBackRectBig = lobbyBackParent->GetChildFind(std::string("lobbyBackRectBig" + std::to_string(r)).c_str());
        auto& rsUI = lobbyBackRectSmall->GetComponent<UiSystem>();
        auto& rbUI = lobbyBackRectBig->GetComponent<UiSystem>();

        //再生していないなら初期化する
        if (rsUI->GetEasingTime() > 1)
        {
            //大きさ
            float sacleR = 0.1f + (rand() % 40) * 0.01f;
            rsUI->spc.scale = { 0 ,0 };
            rbUI->spc.scale = { 0 ,0 };
            rsUI->spc.easingscale = { sacleR * 0.8f ,sacleR * 0.8f };
            rbUI->spc.easingscale = { sacleR ,sacleR };
            //時間
            float timeR = 0.1f + (rand() % 150) * 0.01f;
            rsUI->spc.timescale = timeR;
            rbUI->spc.timescale = timeR;
            //位置
            float posY = 50 + rand() % 980;
            float posX = 50 + rand() % 1820;
            //影遠さ
            float shadowScale = (1920.0f - posX) / 1920.0f;
            rsUI->spc.position = { posX ,posY };
            rbUI->spc.position = { posX ,posY };
            int kakudo = rand() % 4;
            int offSize = 50;
            float offPosSmall = 300;
            if (kakudo == 0)
            {
                rsUI->spc.easingposition = { posX + offSize - (offPosSmall * sacleR) * shadowScale ,posY + offSize + (offPosSmall * sacleR) * shadowScale };
                rbUI->spc.easingposition = { posX + offSize ,posY + offSize };
            }
            if (kakudo == 1)
            {
                rsUI->spc.easingposition = { posX + offSize - (offPosSmall * sacleR) * shadowScale,posY - offSize + (offPosSmall * sacleR) * shadowScale };
                rbUI->spc.easingposition = { posX + offSize ,posY - offSize };
            }
            if (kakudo == 2)
            {
                rsUI->spc.easingposition = { posX - offSize - (offPosSmall * sacleR) * shadowScale ,posY - offSize + (offPosSmall * sacleR) * shadowScale };
                rbUI->spc.easingposition = { posX - offSize ,posY - offSize };
            }
            if (kakudo == 3)
            {
                rsUI->spc.easingposition = { posX - offSize - (offPosSmall * sacleR) * shadowScale,posY + offSize + (offPosSmall * sacleR) * shadowScale };
                rbUI->spc.easingposition = { posX - offSize ,posY + offSize };
            }

            //回転
            rsUI->spc.angle = 0;
            rbUI->spc.angle = 0;
            float posxH = fabsf(posX - 1920 * 0.5f);
            float angle = 120.0f * (posxH / (1920 * 0.5f));
            if (posX > 1920 * 0.5f)angle *= -1;
            rsUI->spc.easingangle = angle + (angle * 0.4f) * shadowScale;
            rbUI->spc.easingangle = angle;
            //色
            float per = ((1920.f - posX) / 1920.0f) * 0.5f + (posY / 1080.0f) * 0.5f;
            DirectX::XMFLOAT3 color = Mathf::Lerp({ 1,1,0 }, { 0,0.5f,1 }, Quart::easeOut(per * per));
            rsUI->spc.color = { 1,1,0,0.0f };
            rbUI->spc.color = { 1,1,0,0.0f };
            rsUI->spc.easingcolor = { color.x,color.y,color.z,0.3f };
            rbUI->spc.easingcolor = { color.x,color.y,color.z,0.8f };

            rsUI->GetEasingTimeReset();
            rbUI->GetEasingTimeReset();
            rsUI->EasingPlay();
            rbUI->EasingPlay();
        }
    }

    for (int c = 0; c < 2; ++c) //円
    {
        auto& lobbyBackCircle = lobbyBackParent->GetChildFind(std::string("lobbyBackCircle" + std::to_string(c)).c_str());
        auto& cUI = lobbyBackCircle->GetComponent<UiSystem>();

        //cUI
        if (c == 0)
            cUI->spc.angle += elapsedTime * (50 + 10 * c);
        else
            cUI->spc.angle -= elapsedTime * (50 + 10 * c);
    }

    for (int b = 0; b < crossNum; b += 2) //バツ
    {
        auto& lobbyBackCrossSmall = lobbyBackParent->GetChildFind(std::string("lobbyBackCross" + std::to_string(b)).c_str());
        auto& lobbyBackCrossBig = lobbyBackParent->GetChildFind(std::string("lobbyBackCross" + std::to_string(b + 1)).c_str());
        auto& csUI = lobbyBackCrossSmall->GetComponent<UiSystem>();
        auto& cbUI = lobbyBackCrossBig->GetComponent<UiSystem>();

        //再生していないなら初期化する
        if (csUI->GetEasingTime() > 1)
        {
            //大きさ
            float sacleR = 0.1f + (rand() % 40) * 0.01f;
            csUI->spc.scale = { 0 ,0 };
            cbUI->spc.scale = { 0 ,0 };
            csUI->spc.easingscale = { sacleR * 0.8f ,sacleR * 0.8f };
            cbUI->spc.easingscale = { sacleR ,sacleR };
            //時間
            float timeR = 0.3f + (rand() % 200) * 0.01f;
            csUI->spc.timescale = timeR;
            cbUI->spc.timescale = timeR;
            //位置
            float posY = 50 + rand() % 980;
            float posX = 50 + rand() % 1820;
            //影遠さ
            float shadowScale = (1920.0f - posX) / 1920.0f;
            csUI->spc.position = { posX ,posY };
            cbUI->spc.position = { posX ,posY };
            int kakudo = rand() % 4;
            int offSize = 50;
            float offPosSmall = 150;
            if (kakudo == 0)
            {
                csUI->spc.easingposition = { posX + offSize - (offPosSmall * sacleR) * shadowScale ,posY + offSize + (offPosSmall * sacleR) * shadowScale };
                cbUI->spc.easingposition = { posX + offSize ,posY + offSize };
            }
            if (kakudo == 1)
            {
                csUI->spc.easingposition = { posX + offSize - (offPosSmall * sacleR) * shadowScale ,posY - offSize + (offPosSmall * sacleR) * shadowScale };
                cbUI->spc.easingposition = { posX + offSize ,posY - offSize };
            }
            if (kakudo == 2)
            {
                csUI->spc.easingposition = { posX - offSize - (offPosSmall * sacleR) * shadowScale ,posY - offSize + (offPosSmall * sacleR) * shadowScale };
                cbUI->spc.easingposition = { posX - offSize ,posY - offSize };
            }
            if (kakudo == 3)
            {
                csUI->spc.easingposition = { posX - offSize - (offPosSmall * sacleR) * shadowScale ,posY + offSize + (offPosSmall * sacleR) * shadowScale };
                cbUI->spc.easingposition = { posX - offSize ,posY + offSize };
            }

            //回転
            csUI->spc.angle = 0;
            cbUI->spc.angle = 0;
            float posxH = fabsf(posX - 1920 * 0.5f);
            float angle = 180.0f * (posxH / (1920 * 0.5f));
            if (posX > 1920 * 0.5f)angle *= -1;
            csUI->spc.easingangle = angle + (angle * 0.4f) * shadowScale;
            cbUI->spc.easingangle = angle;
            //色
            float per = ((1920.f - posX) / 1920.0f) * 0.5f + (posY / 1080.0f) * 0.5f;
            DirectX::XMFLOAT3 color = Mathf::Lerp({ 1,1,0 }, { 0,0.5f,1 }, Quart::easeOut(per * per));
            csUI->spc.color = { 1,1,0,0.0f };
            cbUI->spc.color = { 1,1,0,0.0f };
            csUI->spc.easingcolor = { color.x,color.y,color.z,0.3f };
            cbUI->spc.easingcolor = { color.x,color.y,color.z,0.8f };

            csUI->GetEasingTimeReset();
            cbUI->GetEasingTimeReset();
            csUI->EasingPlay();
            cbUI->EasingPlay();
        }
    }
}