//一番上でインクルード（ネットワーク）
#include "Netwark/Client.h"
#include "Netwark/Server.h"

#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneDebugGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "imgui.h"

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
void SceneDebugGame::Initialize()
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

    //プレイヤー
    players[n->GetNetId()] = GenerateTestCharacter({0,3,0});
    std::shared_ptr<GameObject> cameraPost = players[n->GetNetId()].lock()->AddChildObject();
    cameraPost->SetName("cameraPostPlayer");
    std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();
    cameraPost->transform_->SetWorldPosition({ 0, 950, 300 });
    players[n->GetNetId()].lock()->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());

    //サンドバッグ
    GameObj bot = GameObjectManager::Instance().Create();
    CharacterCom* charaComp = bot->AddComponent<CharacterCom>().get();
    SphereColliderCom* sphere = bot->AddComponent<SphereColliderCom>().get();
    sphere->SetRadius(2.0f);
    sphere->SetMyTag(COLLIDER_TAG::Enemy);
    sphere->SetJudgeTag(COLLIDER_TAG::PlayerAttack);
    bot->AddComponent<MovementCom>();

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
void SceneDebugGame::Finalize()
{
}

// 更新処理
void SceneDebugGame::Update(float elapsedTime)
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
    GameObjectManager::Instance().Update(elapsedTime);
    GameObjectManager::Instance().UpdateTransform();
}

// 描画処理
void SceneDebugGame::Render(float elapsedTime)
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

void SceneDebugGame::SetUserInputs()
{
    // プレイヤーの入力情報
    SetPlayerInput();

    // 他のプレイヤーの入力情報
    SetOnlineInput();
}

void SceneDebugGame::SetPlayerInput()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    //このクライアントの担当プレイヤーがいなかったらやめる
    if (players[n->GetNetId()].use_count() == 0)return;

    std::shared_ptr<CharacterCom> chara = players[n->GetNetId()].lock()->GetComponent<CharacterCom>();
    if (chara.use_count() == 0) return;

    // 入力情報をプレイヤーキャラクターに送信
    chara->SetUserInput(gamePad.GetButton());
    chara->SetUserInputDown(gamePad.GetButtonDown());
    chara->SetUserInputUp(gamePad.GetButtonUp());

    chara->SetLeftStick(gamePad.GetAxisL());
    chara->SetRightStick(gamePad.GetAxisR());
}

void SceneDebugGame::SetOnlineInput()
{
    if (!n)return;

    for (auto& client : n->GetNetDatas())
    {
        if (players[client.id].use_count() != 0)
        {
            std::shared_ptr<CharacterCom> chara = players[client.id].lock()->GetComponent<CharacterCom>();

            if (!chara)continue;

            // 入力情報をプレイヤーキャラクターに送信
            chara->SetUserInput(client.input);
            chara->SetUserInputDown(client.inputDown);
            chara->SetUserInputUp(client.inputUp);

            //ダメージ情報更新
            for (int i = 0; i < MAX_PLAYER_NUM; ++i)
            {
                if (players[i].use_count() == 0) continue;

                players[i].lock()->GetComponent<CharacterCom>()->AddHitPoint(client.damageData[i]);
            }
        }
    }
}

void SceneState_Login::Enter()
{

}
