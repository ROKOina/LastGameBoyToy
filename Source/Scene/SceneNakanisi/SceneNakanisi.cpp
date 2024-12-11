#include "SceneNakanisi.h"
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
#include "Component\System\GameObject.h"

#include "Component/Collsion/NodeCollsionCom.h"

#include "Component/Renderer/InstanceRendererCom.h"

void SceneNakanisi::Initialize()
{
    Graphics& graphics = Graphics::Instance();

#pragma region ゲームオブジェクトの設定

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

    //// Instancingサンプル
    //{
    //    // 生成器
    //    auto& spawnerObj = GameObjectManager::Instance().Create();
    //    spawnerObj->SetName("spawner");
    //    spawnerObj->transform_->SetWorldPosition({ 0.00f, 0.0f, 0.000f });
    //    spawnerObj->transform_->SetScale({ 0.01f,0.01f,0.01f });
    //    std::shared_ptr<InstanceRenderer> ir = spawnerObj->AddComponent<InstanceRenderer>(SHADER_ID_MODEL::DEFERRED, 1, BLENDSTATE::ALPHA);
    //    ir->LoadModel("Data/Model/MatuokaStage/Reactor.mdl");

    //    // 大量生産
    //    for (int i = 0; i < 6; ++i) {
    //        auto& obj = ir->CreateInstance((i % 2 == 0));
    //        obj->SetName("instanceOBJ");
    //        obj->transform_->SetWorldPosition({ 8.0f * (i % 10) - 5, 1.1f, 0.4f * (i / 10) - 5 });
    //    }
    //}

    //ステージ
    {
        auto& stageObj = GameObjectManager::Instance().Create();
        stageObj->SetName("stage");
        stageObj->transform_->SetWorldPosition({ 0.00f, 0.00f, 0.000f });
        stageObj->transform_->SetScale({ 0.05f, 0.05f, 0.05f });
        std::shared_ptr<RendererCom> r = stageObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/AbeStage/TestStage.mdl");
        //r->SetOutlineColor({ 0.000f, 0.932f, 1.000f });
        //r->SetOutlineIntensity(5.5f);

        //ステージ
        StageEditorCom* stageEdit = stageObj->AddComponent<StageEditorCom>().get();
        //判定生成
        stageEdit->PlaceStageRigidCollider("Data/Model/AbeStage/", "testStage.mdl", "__", 0.05);
        //Jsonからオブジェクト配置
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/AbeStage_Spawn.json");
        ////配置したステージオブジェクトの中からGateを取得
        //StageEditorCom::PlaceObject placeObj = stageEdit->GetPlaceObject("Gate");
        //for (auto& obj : placeObj.objList)
        //{
        //    DirectX::XMFLOAT3 pos = obj->transform_->GetWorldPosition();

        //    GateGimmick* gate = obj->GetComponent<GateGimmick>().get();
        //    gate->SetDownPos(pos);
        //    gate->SetUpPos({ pos.x, 1.85f, pos.z });
        //    gate->SetMoveSpeed(0.1f);
        //}
    }

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");

        //配置したステージオブジェクトの中からスポーン位置を取得
        StageEditorCom* edit = GameObjectManager::Instance().Find("stage")->GetComponent<StageEditorCom>().get();
        StageEditorCom::PlaceObject spawnObj = edit->GetPlaceObject("Spawn");

        auto it = spawnObj.objList.begin();
        std::advance(it, 0);
        obj->transform_->SetWorldPosition(it->get()->transform_->GetWorldPosition());
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::JANKRAT, obj);
    }

    //snowparticle
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("snowparticle");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/snow.gpuparticle", 10000);
    }

#pragma endregion

#pragma region グラフィック系の設定

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    //ネット大事
    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);
}

void SceneNakanisi::Finalize()
{
    photonNet->close();
}

//#include "Netwark\Photon\Photon_lib.h"
//extern PhotonLib* photonLib;

void SceneNakanisi::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    //ネット更新
    photonNet->run(elapsedTime);

    ////ホストに繋がった瞬間に位置を変える
    //if (photonLib->GetConnectNow())
    //{
    //    //配置したステージオブジェクトの中からスポーン位置を取得
    //    StageEditorCom* edit = GameObjectManager::Instance().Find("stage")->GetComponent<StageEditorCom>().get();
    //    StageEditorCom::PlaceObject spawnObj = edit->GetPlaceObject("Spawn");

    //    GameObj player = GameObjectManager::Instance().Find("player");

    //    auto it = spawnObj.objList.begin();
    //    std::advance(it, player->GetComponent<CharacterCom>()->GetNetCharaData().GetNetPlayerID());

    //    player->transform_->SetWorldPosition(it->get()->transform_->GetWorldPosition());
    //}

    //イベントカメラ用
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    //UI生成
    PlayerUIManager::Instance().UIUpdate(elapsedTime);

    //ゲームオブジェクトの行列更新
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

void SceneNakanisi::Render(float elapsedTime)
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
    RegisterChara::Instance().ImGui();
#endif

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());

    //imgui
    photonNet->ImGui();

    //イベントカメラ用
    EventCameraManager::Instance().EventCameraImGui();
}

//オブジェクト生成関数
void SceneNakanisi::NewObject()
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