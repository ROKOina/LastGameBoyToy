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
#include "Components\ColliderCom.h"
#include "Components\MovementCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"
#include "Components\Character\UenoCharacterCom.h"
#include "Components\Character\NomuraCharacterCom.h"
#include "Components/CPUParticle.h"
#include "GameSource/GameScript/FreeCameraCom.h"
#include "GameSource/GameScript/FPSCameraCom.h"
#include "Components/CPUParticle.h"
#include "Components/GPUParticle.h"

#include "Components\Character\Generate\TestCharacterGenerate.h"

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

    //GameObj testPlayer = GenerateTestCharacter({ 6,3,0 });

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/OneCoin/robot.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        a->PlayAnimation(0, true, false, 0.001f);
        std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
        //std::shared_ptr<InazawaCharacterCom> c = obj->AddComponent<InazawaCharacterCom>();
        std::shared_ptr<TestCharacterCom> c = obj->AddComponent<TestCharacterCom>();
        //std::shared_ptr<UenoCharacterCom> c = obj->AddComponent<UenoCharacterCom>();
        //std::shared_ptr<NomuraCharacterCom> c = obj->AddComponent<NomuraCharacterCom>();

        std::shared_ptr<SphereColliderCom> sphere = obj->AddComponent<SphereColliderCom>();
        sphere->SetMyTag(COLLIDER_TAG::Player);
        sphere->SetRadius(2.0f);
    }

    //カメラをプレイヤーの子どもにして制御する
    {
        std::shared_ptr<GameObject> playerObj = GameObjectManager::Instance().Find("player");
        std::shared_ptr<GameObject> cameraPost = playerObj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();
        cameraPost->transform_->SetWorldPosition({ 0, 950, 300 });
        playerObj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());
    }

    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, 3.7f, 0 });
        obj->transform_->SetScale({ 0.8f, 0.8f, 0.8f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/canyon/stage.mdl");
        obj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");
    }

    //テスト
    {
        //Graphics::Instance().SetWorldSpeed(0.1f);
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("SciFiGate");
        obj->transform_->SetWorldPosition({ 0, 1.8f, 5 });
        obj->transform_->SetScale({ 0.06f,0.0001f,0.02f });
        obj->transform_->SetEulerRotation({ 90,0,0 });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::SCI_FI_GATE, BLENDSTATE::ADD, RASTERIZERSTATE::SOLID_CULL_NONE);
        r->LoadModel("Data/UtilityModels/plane.mdl");
        r->LoadMaterial("Data/UtilityModels/SciFiGate.Material");

        auto& cb = r->SetVariousConstant<SciFiGateConstants>();
        cb->simulateSpeed1 = 1.1f;
        cb->simulateSpeed2 = -0.3f;
        cb->uvScrollDir1 = { 1.0f,1.0f };
        cb->uvScrollDir2 = { 0.0f,1.0f };
        cb->uvScale1 = { 2.0f,1.0f };
        cb->intensity1 = 0.8f;
        cb->intensity2 = 1.6f;
        cb->effectColor1 = { 1.0f,0.4f,0.0f,1.0f };
        cb->effectColor2 = { 1.0f,0.2f,0.0f,1.0f };
        cb->contourIntensity = 1.5f;
    }
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("test");
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFAULT, BLENDSTATE::ADD, RASTERIZERSTATE::SOLID_CULL_NONE);
        r->LoadModel("Data/Ball/b.mdl");
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
}

// 終了化
void SceneGame::Finalize()
{
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

    // キーの入力情報を各キャラクターに割り当てる
    SetUserInputs();

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
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //コンスタントバッファの更新
    ConstantBufferUpdate(elapsedTime);

    //サンプラーステートの設定
    Graphics::Instance().SetSamplerState();

    // ライトの定数バッファを更新
    LightManager::Instance().UpdateConstatBuffer();

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, mainDirectionalLight->GetDirection());

    //オブジェクト描画
    GameObjectManager::Instance().DrawGuizmo(sc->data.view, sc->data.projection);

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
        ////自分自身の場合は入力情報を更新
        //if (client.id == n->GetNetId())
        //{
        //    GamePad& gamePad = Input::Instance().GetGamePad();

        //    client.input |= gamePad.GetButton();
        //    client.inputDown |= gamePad.GetButtonDown();
        //    client.inputUp |= gamePad.GetButtonUp();
        //}

        std::string name = "Net" + std::to_string(client.id);
        std::shared_ptr<GameObject> clientObj = GameObjectManager::Instance().Find(name.c_str());

        if (clientObj)
        {
            std::shared_ptr<CharacterCom> chara = clientObj->GetComponent<CharacterCom>();

            if (!chara)continue;

            // 入力情報をプレイヤーキャラクターに送信
            chara->SetUserInput(client.input);
            chara->SetUserInputDown(client.inputDown);
            chara->SetUserInputUp(client.inputUp);

            DirectX::XMFLOAT3 velocity = Mathf::Normalize(client.velocity);

            DirectX::XMFLOAT2 leftS = { velocity.x,velocity.z };

            chara->SetLeftStick(leftS);
            //chara->SetRightStick(gamePad.GetAxisR());
        }
    }
}

void SceneGame::DelayOnlineInput()
{
    if (!n)return;

    //for (auto& netClient : n->GetNetDatas())
    //{
    //    netClient.id
    //}
}
