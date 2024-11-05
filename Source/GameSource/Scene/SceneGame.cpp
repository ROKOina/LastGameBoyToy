//一番上でインクルード（ネットワーク）
#include "Netwark/Client.h"
#include "Netwark/Server.h"

#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "imgui.h"

#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\RayCollisionCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Components\AnimationCom.h"
#include "Components\AimIKCom.h"
#include "Components\MovementCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"
#include "Components\Character\UenoCharacterCom.h"
#include "Components\Character\NomuraCharacterCom.h"
#include "Components\Character\HaveAllAttackCharacter.h"
#include "Components\Character\RegisterChara.h"
#include "Components/CPUParticle.h"
#include "Components\FootIKcom.h"
#include "GameSource/GameScript/FreeCameraCom.h"
#include "GameSource/GameScript/FPSCameraCom.h"
#include "GameSource/GameScript/EventCameraCom.h"
#include "Components/CPUParticle.h"
#include "Components/GPUParticle.h"
#include "Graphics/Sprite/Sprite.h"
#include "Components/StageEditorCom.h"
#include "Components/SpawnCom.h"
#include "Components/Enemy/Boss/BossCom.h"
#include "Components/InstanceRendererCom.h"
#include "Components\EasingMoveCom.h"

#include "Components\Character\Generate\TestCharacterGenerate.h"

#include "Netwark/Photon/StdIO_UIListener.h"

#include "GameSource/GameScript/EventCameraManager.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include <Components/Character/CharaStatusCom.h>

#include "Phsix\Physxlib.h"
#include "Components\RigidBodyCom.h"

#include "Audio/AudioSource.h"

SceneGame::~SceneGame()
{
    GameObjectManager::Instance().AllRemove();
    GameObjectManager::Instance().RemoveGameObjects();
}

// 初期化
void SceneGame::Initialize()
{
    Graphics& graphics = Graphics::Instance();

#pragma region ゲームオブジェクトの設定

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //イベント用カメラ
    {
        std::shared_ptr<GameObject> eventCamera = GameObjectManager::Instance().Create();
        eventCamera->SetName("eventcamera");
        eventCamera->AddComponent<EventCameraCom>();
        eventCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }


    ModelResource* m;
    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, 3.7f, 0 });
        obj->transform_->SetScale({ 0.01f, 0.01f, 0.08f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::STAGEDEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        //r->LoadModel("Data/canyon/stage.mdl");
        r->LoadModel("Data/MatuokaStage/StageJson/StageJson.mdl");
        obj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");
        obj->AddComponent<NodeCollsionCom>("Data/Stage_Abe/test.nodecollsion");
        obj->AddComponent<SphereColliderCom>()->SetMyTag(COLLIDER_TAG::Enemy);
        //obj->AddComponent<NodeCollsionCom>(nullptr);
        obj->AddComponent<StageEditorCom>();
        PhysXLib::Instance().GenerateManyCollider(r->GetModel()->GetResource());
        //RigidBodyCom* rigid = obj->AddComponent<RigidBodyCom>(true, NodeCollsionCom::CollsionType::SPHER).get();
        //rigid->GenerateCollider(r->GetModel()->GetResource());
        //m = r->GetModel()->GetResource();
    }

    //当たり判定用
    std::shared_ptr<GameObject> roboobj = GameObjectManager::Instance().Create();
    {
        roboobj->SetName("robo");
        roboobj->transform_->SetWorldPosition({ 0, 0, 0 });
        roboobj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
        std::shared_ptr<RendererCom> r = roboobj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/OneCoin/robot.mdl");
        std::shared_ptr<AnimationCom> a = roboobj->AddComponent<AnimationCom>();
        a->PlayAnimation(0, true, false, 0.001f);

        //std::shared_ptr<SphereColliderCom> sphere = roboobj->AddComponent<SphereColliderCom>();
        //sphere->SetRadius(2.0f);
        //sphere->SetMyTag(COLLIDER_TAG::Enemy);
        //sphere->SetJudgeTag(COLLIDER_TAG::Player);

        roboobj->AddComponent<NodeCollsionCom>("Data/OneCoin/OneCoin.nodecollsion");
        RigidBodyCom* rigid = roboobj->AddComponent<RigidBodyCom>(false,NodeCollsionCom::CollsionType::SPHER).get();
        rigid->GenerateCollider(NodeCollsionCom::CollsionType::SPHER);
    }

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::HAVE_ALL_ATTACK, obj);
    }

    //カメラをプレイヤーの子どもにして制御する
    {
        std::shared_ptr<GameObject> playerObj = GameObjectManager::Instance().Find("player");
        std::shared_ptr<GameObject> cameraPost = playerObj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();

        //pico位置
        cameraPost->transform_->SetWorldPosition({ 0, 80.821f, 33.050f });
        playerObj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());
    }

    //BOSS
    {
        auto& boss = GameObjectManager::Instance().Create();
        boss->SetName("BOSS");
        std::shared_ptr<RendererCom> r = boss->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Jammo/jammo.mdl");
        boss->transform_->SetWorldPosition({ 0.0f,0.0f,14.0f });
        boss->transform_->SetScale({ 0.06f, 0.06f, 0.06f });
        boss->AddComponent<EasingMoveCom>(nullptr);
        //t = boss->transform_;
        boss->AddComponent<MovementCom>();
        boss->AddComponent<NodeCollsionCom>("Data/Jammo/jammocollsion.nodecollsion");
        boss->AddComponent<AnimationCom>();
        boss->AddComponent<BossCom>();
        boss->AddComponent<AimIKCom>(nullptr, "mixamorig:Neck");
        boss->AddComponent<CharaStatusCom>();
        //boss->AddComponent<SpawnCom>();
    }

    //インスタンステスト
    {
        //auto& obj = GameObjectManager::Instance().Create();
        //obj->SetName("Instance");
        //obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
        //std::shared_ptr<InstanceRenderer> r = obj->AddComponent<InstanceRenderer>(SHADER_ID_MODEL::DEFERRED, 2, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true);
        //r->LoadModel("Data/Jammo/jammo.mdl");
    }

    //UI
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");
        obj->AddComponent<Sprite>("Data/UIData/reticle.ui", false);
    }

#pragma endregion

#pragma region グラフィック系の設定
    //平行光源を追加
    mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);

    // スカイボックスの設定
    std::array<const char*, 4> filepath = {
      "Data\\Texture\\snowy_hillside_4k.DDS",
      "Data\\Texture\\diffuse_iem.dds",
      "Data\\Texture\\specular_pmrem.dds",
      "Data\\Texture\\lut_ggx.DDS"
    };
    SkyBoxManager::Instance().LoadSkyBoxTextures(filepath);

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

#pragma endregion

#pragma region オーディオ系の設定
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("Audio Test SE");
    std::shared_ptr<AudioSource> audio = obj->AddComponent<AudioSource>();
    audio->SetAudio(static_cast<int>(AUDIOID::SE));
    audio->Play(false, 0.5f);
    audio->SetAudioName("Test");

#pragma endregion

    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);
}

// 終了化
void SceneGame::Finalize()
{
    photonNet->close();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
    if (n)
    {
        n->Update();

        if (!n->IsNextFrame())
        {
            return;
        }
    }

    photonNet->run(elapsedTime);

    //イベントカメラ用
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    //ボスの位置取得
    //sc->data.bossposiotn = t->GetWorldPosition();

    // ゲームオブジェクトの更新
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render(float elapsedTime)
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

    // ライトの定数バッファを更新
    LightManager::Instance().UpdateConstatBuffer();

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, mainDirectionalLight->GetDirection());

    if (n)
        n->ImGui();
    else
    {
        //ネットワーク決定仮ボタン
        ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

        ImGui::Begin("NetSelect", nullptr, ImGuiWindowFlags_None);

        static int ClientID = 0;
        static std::string ip;
        char ipAdd[256];

        ImGui::InputInt("id", &ClientID);
        ::strncpy_s(ipAdd, sizeof(ipAdd), ip.c_str(), sizeof(ipAdd));
        if (ImGui::InputText("ipv4Adress", ipAdd, sizeof(ipAdd), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ip = ipAdd;
        }
        if (ImGui::Button("Client"))
        {
            if (ip.size() > 0)
            {
                n = std::make_unique<NetClient>(ip, ClientID);
                n->Initialize();
            }
        }

        if (ImGui::Button("Server"))
        {
            n = std::make_unique<NetServer>();
            n->Initialize();
        }

        ImGui::End();
    }

    photonNet->ImGui();

    //イベントカメラ用
    EventCameraManager::Instance().EventCameraImGui();
}

void SceneGame::SetUserInputs()
{
    // プレイヤーの入力情報
    SetPlayerInput();

    // 他のプレイヤーの入力情報
    SetOnlineInput();
}

void SceneGame::SetPlayerInput()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("player");
    if (obj.use_count() == 0)return;

    std::shared_ptr<CharacterCom> chara = obj->GetComponent<CharacterCom>();
    if (chara.use_count() == 0) return;

    // 入力情報をプレイヤーキャラクターに送信
    chara->SetUserInput(gamePad.GetButton());
    chara->SetUserInputDown(gamePad.GetButtonDown());
    chara->SetUserInputUp(gamePad.GetButtonUp());

    chara->SetLeftStick(gamePad.GetAxisL());
    chara->SetRightStick(gamePad.GetAxisR());
}

void SceneGame::SetOnlineInput()
{
    if (!n)return;

    for (auto& client : n->GetNetDatas())
    {
        std::string name = "Net" + std::to_string(client.id);
        std::shared_ptr<GameObject> clientObj = GameObjectManager::Instance().Find(name.c_str());

        if (clientObj)
        {
            std::shared_ptr<CharacterCom> chara = clientObj->GetComponent<CharacterCom>();

            if (!chara)continue;
        }
    }
}

void SceneGame::DelayOnlineInput()
{
    if (!n)return;
}