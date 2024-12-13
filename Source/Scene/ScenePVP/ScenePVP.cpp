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

#include "Component/Renderer/InstanceRendererCom.h"

#include "Netwark/Photon/Photon_lib.h"
#include "../SceneTitle/SceneTitle.h"


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
        obj->AddComponent<Light>(nullptr);
    }

    //ロビー選択から始まる
    //InitializePVP();
    InitializeLobbySelect();

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    //ネット大事
    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);

    //ゲームシステム
    pvpGameSystem = std::make_unique<PVPGameSystem>();
}

void ScenePVP::InitializeLobbySelect()
{

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
            strBack.emplace_back(kariLOBBYSELECT);  //背景管理用
        }

        std::shared_ptr<GameObject> obj = FParent->AddChildObject();
        //std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName(("lobbySelectFont" + std::to_string(lf.id)).c_str());
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = lf.pos;
        font->str = lf.str;  //L付けてね
        font->scale = lf.scale;
        //削除予定リストに追加
        tempRemoveObj.emplace_back(obj);
    }
}

void ScenePVP::InitializeLobby()
{
    //kari
    {
        std::shared_ptr<GameObject> kariLOBBY = GameObjectManager::Instance().Create();
        kariLOBBY->SetName("kariLobby");
        kariLOBBY->AddComponent<UiSystem>("Data/Texture/LobbyPVPUI/kariLOBBY.ui", Sprite::SpriteShader::DEFALT, false);
        //削除予定リストに追加
        tempRemoveObj.emplace_back(kariLOBBY);
    }


}

void ScenePVP::InitializeCharaSelect()
{
    //kari
    {
        std::shared_ptr<GameObject> kariCHARASELECT = GameObjectManager::Instance().Create();
        kariCHARASELECT->SetName("kariCHARASELECT");
        kariCHARASELECT->AddComponent<UiSystem>(nullptr, Sprite::SpriteShader::DEFALT, false);
        //削除予定リストに追加
        tempRemoveObj.emplace_back(kariCHARASELECT);
    }
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

    //イベント用カメラ
    {
        std::shared_ptr<GameObject> eventCamera = GameObjectManager::Instance().Create();
        eventCamera->SetName("eventcamera");
        eventCamera->AddComponent<EventCameraCom>();
        eventCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //ステージ
    {
        auto& stageObj = GameObjectManager::Instance().Create();
        stageObj->SetName("stage");
        stageObj->transform_->SetWorldPosition({ 0, 0, 0 });
        stageObj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        std::shared_ptr<RendererCom> r = stageObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");
        r->SetOutlineColor({ 0.000f, 0.932f, 1.000f });
        r->SetOutlineIntensity(5.5f);
        stageObj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");

        //ステージ
        StageEditorCom* stageEdit = stageObj->AddComponent<StageEditorCom>().get();
        //判定生成
        stageEdit->PlaceStageRigidCollider("Data/Model/MatuokaStage/", "StageJson/ColliderStage.mdl", "__", 0.005f);
        //Jsonからオブジェクト配置
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/GateGimic.json");
        //配置したステージオブジェクトの中からGateを取得
        StageEditorCom::PlaceObject placeObj = stageEdit->GetPlaceObject("Gate");
        for (auto& obj : placeObj.objList)
        {
            DirectX::XMFLOAT3 pos = obj->transform_->GetWorldPosition();

            GateGimmick* gate = obj->GetComponent<GateGimmick>().get();
            gate->SetDownPos(pos);
            gate->SetUpPos({ pos.x, 1.85f, pos.z });
            gate->SetMoveSpeed(0.1f);
        }
    }

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0,0,0 });
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::FARAH, obj);
    }

    //snowparticle
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("snowparticle");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/snow.gpuparticle", 10000);
    }

    //Font参考例
    //font
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testFont");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 0,0 };
        font->str = L"ab";  //L付けてね
        font->scale = 1.0f;
    }

#pragma endregion
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

void ScenePVP::CreateUiObject()
{
    //UI
    {
        //キャンバス
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");

        //レティクル
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> reticle = canvas->AddChildObject();
            reticle->SetName("reticle");
            reticle->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Reticle.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //HpFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpFrame = canvas->AddChildObject();
            hpFrame->SetName("HpFrame");
            hpFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/HpFrame.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //HpGauge
        {
            std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
            std::shared_ptr<GameObject> hpGauge = hpFrame->AddChildObject();
            hpGauge->SetName("HpGauge");
            std::shared_ptr<UiGauge>gauge = hpGauge->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/HpGauge.ui", Sprite::SpriteShader::DEFALT, true, UiSystem::X_ONLY_ADD);
            gauge->SetMaxValue(GameObjectManager::Instance().Find("player")->GetComponent<CharaStatusCom>()->GetMaxHitpoint());
            float* i = GameObjectManager::Instance().Find("player")->GetComponent<CharaStatusCom>()->GetHitPoint();
            gauge->SetVariableValue(i);
        }

        //UltFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("UltFrame");
            std::shared_ptr<UiSystem> fade = hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltFrame.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //UltHideGauge
        {
            std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
            std::shared_ptr<GameObject> ultHideGauge = ultFrame->AddChildObject();
            ultHideGauge->SetName("UltHideGauge");
            ultHideGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltHideGauge.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //UltGauge
        {
            std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
            std::shared_ptr<GameObject> ultGauge = ultFrame->AddChildObject();
            ultGauge->SetName("UltGauge");

            std::shared_ptr<UI_Skill>ultGaugeCmp = ultGauge->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/UltGauge.ui", Sprite::SpriteShader::DEFALT, false, 1084, 890);
            std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
            ultGaugeCmp->SetMaxValue(player->GetComponent<CharacterCom>()->GetUltGaugeMax());
            float* i = player->GetComponent<CharacterCom>()->GetUltGauge();
            ultGaugeCmp->SetVariableValue(i);
        }

        //Ultカウント
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> ultCore = canvas->AddChildObject();
            ultCore->SetName("ultCore");
            //int value = GameObjectManager::Instance().Find("player")->GetComponent<InazawaCharacterCom>()->GetRMaxCount();
            //ultCore->AddComponent<UI_Ult_Count>(value);
        }

        ////////////<SKill_E>/////////////////////////////

      //SkillFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> skillFrame = canvas->AddChildObject();
            skillFrame->SetName("SkillFrame");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame1_01.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //SkillFrame2
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skill_Q = SkillFrame->AddChildObject();
            skill_Q->SetName("Skill_Frame2");
            skill_Q->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame1_02.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillMask
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGaugeHide");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrameHide1.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillGauge
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGauge");
            std::shared_ptr<UI_Skill>skillGauge = skillFrame->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/SkillGauge1.ui", Sprite::SpriteShader::DEFALT, false, 1084, 997);
            std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");

            skillGauge->SetMaxValue(player->GetComponent<CharacterCom>()->GetSkillCoolTime(CharacterCom::SkillCoolID::E));
            float* i = player->GetComponent<CharacterCom>()->GetSkillCoolTimerPointer(CharacterCom::SkillCoolID::E);
            skillGauge->SetVariableValue(i);
        }

        //Skill_E
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillGauge = SkillFrame->AddChildObject();
            skillGauge->SetName("Skill_E");
            skillGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_E.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //Skillカウント
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> skillGauge = canvas->AddChildObject();
            skillGauge->SetName("SkillCore");
            skillGauge->AddComponent<UI_E_SkillCount>(8);
        }

        ////////////////<Skill_Space>/////////////////////////////////////

             //SkillFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> skillFrame = canvas->AddChildObject();
            skillFrame->SetName("SkillFrame2");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame2_01.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillFrame2
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skill_Q = SkillFrame->AddChildObject();
            skill_Q->SetName("Skill_Frame2");
            skill_Q->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame2_02.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillMask
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGaugeHide");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrameHide2.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillGauge
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGauge");
            std::shared_ptr<UI_Skill>skillGauge = skillFrame->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/SkillGauge2.ui", Sprite::SpriteShader::DEFALT, false, 1030, 937);
            std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
            skillGauge->SetMaxValue(player->GetComponent<CharacterCom>()->GetSkillCoolTime(CharacterCom::SkillCoolID::Space));
            float* i = player->GetComponent<CharacterCom>()->GetSkillCoolTimerPointer(CharacterCom::SkillCoolID::Space);
            skillGauge->SetVariableValue(i);
        }

        //Skill_Space
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skillGauge = SkillFrame->AddChildObject();
            skillGauge->SetName("Skill_SPACE");
            skillGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_SPACE.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //Boost
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("boostGauge2");

            hpMemori->AddComponent<UI_BoosGauge>(2);
        }

        //LockOn
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("lockOn");

            hpMemori->AddComponent<UI_LockOn>(4, 0, 90);
        }
        //decoration
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> skillGauge = SkillFrame->AddChildObject();
            skillGauge->SetName("Decoration");
            skillGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Decoration.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //HitEffect
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("HitEffect");

            bool* flag = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetIsHitAttack();
            hpMemori->AddComponent<UiFlag>("Data/SerializeData/UIData/Player/HitEffect.ui", Sprite::SpriteShader::DEFALT, false, flag);
        }
    }
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

void ScenePVP::TransitionUpdate(float elapsedTime)
{
    switch (lobbyState)
    {
    case 0: //ロビー選択
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
                            auto& back = fP->GetChildFind(("FontBack" + std::to_string(f.id)).c_str()); //背景オブジェクト
                            if (!back)continue;
                            auto& ui = back->GetComponent<UiSystem>();
                            ui->SetHitSprite(false);
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
                //ui->spc.position.y = 218 + 500 * (count - 20);
                font->position.y = 218 + 50 * (count - 20);
                font->str = room;
                //ui->spc.scale.x = 0.096f / 2 * font->str.length() * font->scale;

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
                    ui->spc.color.x = 1;
                }
                else
                    ui->spc.color.x = 0;

                if (joinRoomCount == count)
                    ui->spc.color.x = 1;

                count++;
            }
        }

    }
    break;
    case 1: //ロビー
        break;
    case 2: //キャラ選択
        break;
    case 3: //ゲーム中
        break;
    }
}
    
    
