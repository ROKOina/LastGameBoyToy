#include "SceneTraining.h"
#include  "Scene/SceneTraining/TrainingManager.h"
#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "Input\GamePad.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneLoading/SceneLoading.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component\Renderer\RendererCom.h"
#include "Component/System/TransformCom.h"
#include "Component\Camera\CameraCom.h"
#include "Component\Animation\AnimationCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include "Component\Collsion\RayCollisionCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "Netwark/Photon/StdIO_UIListener.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include "Component\Light\LightCom.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\Stage\StageEditorCom.h"
#include "Component\Collsion\PushBackCom.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Character\RegisterChara.h"
#include "Component\System\HitProcessCom.h"
#include "Component\Camera\FPSCameraCom.h"
#include "Component\Particle\GPUParticle.h"
#include <Component/Character/CharaStatusCom.h>
#include <Component/Audio/AudioCom.h>
#include <Component/UI/UiFlag.h>
#include <Component/UI/UiGauge.h>
#include <Component/UI/PlayerUI.h>

void SceneTraining::Initialize()
{
    //ポストエフェクト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        obj->AddComponent<PostEffect>();
    }

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //ライト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>(nullptr);
    }

    //ステージ
    {
        auto& stageObj = GameObjectManager::Instance().Create();
        stageObj->SetName("stage");
        stageObj->transform_->SetWorldPosition({ 0, 0, 0 });
        stageObj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        std::shared_ptr<RendererCom> r = stageObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");

        //ステージ
        StageEditorCom* stageEdit = stageObj->AddComponent<StageEditorCom>().get();
        //判定生成
        stageEdit->PlaceStageRigidCollider("Data/Model/MatuokaStage/", "StageJson/ColliderStage.mdl", "__", 0.005f);
    }

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0,1,0 });
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::INAZAWA, obj);
    }

    //snowparticle
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("snowparticle");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/snow.gpuparticle", 10000);
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    TrainingManager::Instance().TrainingManagerStart();

    //ネット大事
    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);
}

void SceneTraining::Finalize()
{
    photonNet->close();
    TrainingManager::Instance().TrainingManagerClear();
    GameObjectManager::Instance().AllRemove();
    GameObjectManager::Instance().RemoveGameObjects();
}

void SceneTraining::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    photonNet->run(elapsedTime);

    //UI生成
    PlayerUIManager::Instance().UIUpdate(elapsedTime);

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
    TrainingManager::Instance().TrainingManagerUpdate(elapsedTime);
}

void SceneTraining::Render(float elapsedTime)
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

    photonNet->ImGui();

    TrainingManager::Instance().OnGUI();

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());
}