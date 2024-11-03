//一番上でインクルード（ネットワーク）
#include "Netwark/Client.h"
#include "Netwark/Server.h"

#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneSugimoto.h"
#include "SceneSugimotoEditor.h"
#include "../SceneManager.h"
#include "../SceneLoading.h"
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

#include "GameSource\Scene\SceneResult\SceneResult.h"
#include "Components/FrustumCom.h"

SceneSugimotoEditor::~SceneSugimotoEditor()
{
  GameObjectManager::Instance().AllRemove();
  GameObjectManager::Instance().RemoveGameObjects();
}

// 初期化
void SceneSugimotoEditor::Initialize()
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

  //ステージ
  {
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("stage");
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/canyon/stage.mdl");
    obj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");
    obj->AddComponent<StageEditorCom>();
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
void SceneSugimotoEditor::Finalize()
{
}

// 更新処理
void SceneSugimotoEditor::Update(float elapsedTime)
{
  //イベントカメラ用
  EventCameraManager::Instance().EventUpdate(elapsedTime);

  // ゲームオブジェクトの更新
  GameObjectManager::Instance().UpdateTransform();
  GameObjectManager::Instance().Update(elapsedTime);

  GamePad& gamePad = Input::Instance().GetGamePad();
  if (GamePad::BTN_A & gamePad.GetButtonDown())
  {
    SceneManager::Instance().ChangeScene(new SceneSugimoto);
  }
}

// 描画処理
void SceneSugimotoEditor::Render(float elapsedTime)
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

}

void SceneSugimotoEditor::SetUserInputs()
{
  // プレイヤーの入力情報
  SetPlayerInput();

  // 他のプレイヤーの入力情報
  SetOnlineInput();
}

void SceneSugimotoEditor::SetPlayerInput()
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

void SceneSugimotoEditor::SetOnlineInput()
{
}

void SceneSugimotoEditor::DelayOnlineInput()
{
}

void SceneSugimotoEditor::CreateUiObject()
{
}

void SceneSugimotoEditor::TransitionPVEFromResult()
{
}