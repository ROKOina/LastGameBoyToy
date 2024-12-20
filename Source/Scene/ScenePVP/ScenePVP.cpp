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

#include "PvPUi/CharaPicks.h"

void ScenePVP::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //�|�X�g�G�t�F�N�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        obj->AddComponent<PostEffect>();
    }

    //���C�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>(nullptr);
    }

    //���r�[�I������n�܂�
    //InitializePVP();
    InitializeLobbySelect();

    InitializeCharaSelect();

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

    //�l�b�g�厖
    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);

    //�Q�[���V�X�e��
    pvpGameSystem = std::make_unique<PVPGameSystem>();
}

void ScenePVP::InitializeLobbySelect()
{
    //kari
    {
        std::shared_ptr<GameObject> kariLOBBYSELECT = GameObjectManager::Instance().Create();
        kariLOBBYSELECT->SetName("kariLOBBYSELECT");
        kariLOBBYSELECT->AddComponent<UiSystem>(nullptr, Sprite::SpriteShader::DEFALT, false);
        //�폜�\�胊�X�g�ɒǉ�
        tempRemoveObj.emplace_back(kariLOBBYSELECT);
    }
}

void ScenePVP::InitializeLobby()
{
    //kari
    {
        std::shared_ptr<GameObject> kariLOBBY = GameObjectManager::Instance().Create();
        kariLOBBY->SetName("kariLobby");
        kariLOBBY->AddComponent<UiSystem>("Data/Texture/LobbyPVPUI/kariLOBBY.ui", Sprite::SpriteShader::DEFALT, false);
        //�폜�\�胊�X�g�ɒǉ�
        tempRemoveObj.emplace_back(kariLOBBY);
    }
}

void ScenePVP::InitializeCharaSelect()
{

    charaPicks = std::make_shared<CharaPicks>();

    // �L�����s�b�NUI����
    charaPicks->CreateCharaPicksUiObject();

    //kari
    {
        std::shared_ptr<GameObject> kariCHARASELECT = GameObjectManager::Instance().Create();
        kariCHARASELECT->SetName("kariCHARASELECT");
        kariCHARASELECT->AddComponent<UiSystem>(nullptr, Sprite::SpriteShader::DEFALT, false);
        //�폜�\�胊�X�g�ɒǉ�
        tempRemoveObj.emplace_back(kariCHARASELECT);
    }
}

void ScenePVP::InitializePVP()
{
#pragma region �Q�[���I�u�W�F�N�g�̐ݒ�

#ifdef _DEBUG
    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -103 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();
#endif

    //�C�x���g�p�J����
    {
        std::shared_ptr<GameObject> eventCamera = GameObjectManager::Instance().Create();
        eventCamera->SetName("eventcamera");
        eventCamera->AddComponent<EventCameraCom>();
        eventCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //�X�e�[�W
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

        //�X�e�[�W
        StageEditorCom* stageEdit = stageObj->AddComponent<StageEditorCom>().get();
        //���萶��
        stageEdit->PlaceStageRigidCollider("Data/Model/MatuokaStage/", "StageJson/ColliderStage.mdl", "__", 0.005f);
        //Json����I�u�W�F�N�g�z�u
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/GateGimic.json");
        //�z�u�����X�e�[�W�I�u�W�F�N�g�̒�����Gate���擾
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

    //�v���C���[
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

    //Font�Q�l��
    //font
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testFont");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 0,0 };
        font->str = L"ab";  //L�t���Ă�
        font->scale = 1.0f;
        font->color = { 0,1,0,0.5 };
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
        //�ۑ��I�u�W�F�N�g���ׂč폜
        GameObjectManager::Instance().Remove(removeObj.lock());
    }
    removeObjs.clear();
}

void ScenePVP::Update(float elapsedTime)
{
    //�l�b�g�X�V
    photonNet->run(elapsedTime);

    //�C�x���g�J�����p
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    //Ui�X�V
    PlayerUIManager::Instance().UIUpdate(elapsedTime);

    //�Q�[���V�X�e���X�V
    GameSystemUpdate(elapsedTime);
    pvpGameSystem->update(elapsedTime);

    //�I���
    if (pvpGameSystem->IsGameEnd())
    {
        //���J��
        if (!SceneManager::Instance().GetTransitionFlag())
            SceneManager::Instance().ChangeSceneDelay(new SceneTitle, 2);
    }

    //��ʐ؂�ւ�����
    auto net = photonNet->GetPhotonLib();
    bool lobbyOneFlg = false;   //��񂾂�����������悤��(���r�[��)
    if (!isLobby) //��񂾂�����
    {
        if (net->IsJoinPermission() || net->GetIsMasterPlayer())    //�l�b�g�Ɍq�����������r�[�ɓ���
        {
            lobbyOneFlg = true;
            isLobby = true;
            TransitionRemove(tempRemoveObj);
        }
    }
    bool lobbySelectOneFlg = false;   //��񂾂�����������悤��(�L�����Z���N�g��)
    if (!isCharaSelect) //��񂾂�����
    {
        if (net->GetIsCharaSelect())    //�L�����Z���N�g
        {
            lobbySelectOneFlg = true;
            isCharaSelect = true;
            TransitionRemove(tempRemoveObj);
        }
    }
    bool GameOneFlg = false;   //��񂾂�����������悤��(�Q�[���J�n��)
    if (!isGame) //��񂾂�����
    {
        if (net->GetIsGamePlay())    //�Q�[���J�n
        {
            GameOneFlg = true;
            isGame = true;
            TransitionRemove(tempRemoveObj);

            //�Q�[�����[�h�ݒ�
            pvpGameSystem->SetGameMode(PVPGameSystem::GAME_MODE(net->GetGameMode()));
        }
    }

    //�Q�[���I�u�W�F�N�g�̍s��X�V
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);

    if (lobbyOneFlg)    //��񂾂�����
    {
        InitializeLobby();
    }
    if (lobbySelectOneFlg)    //��񂾂�����
    {
        InitializeCharaSelect();
    }
    if (GameOneFlg)    //��񂾂�����
    {
        InitializePVP();
    }

    // �L�����s�b�N�X�V����
    charaPicks->CharaPicksUpdate(elapsedTime);
}

void ScenePVP::Render(float elapsedTime)
{
    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //�R���X�^���g�o�b�t�@�̍X�V
    ConstantBufferUpdate(elapsedTime);

    //�T���v���[�X�e�[�g�̐ݒ�
    Graphics::Instance().SetSamplerState();

    //�I�u�W�F�N�g�����֐�
#ifdef _DEBUG
    NewObject();
#endif

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());

    //imgui
    photonNet->ImGui();

    //�C�x���g�J�����p
    EventCameraManager::Instance().EventCameraImGui();
}

//�I�u�W�F�N�g�����֐�
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
    //�e�Q�[�����[�h�K�v���X�V
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