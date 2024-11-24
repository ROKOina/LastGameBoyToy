#include "ScenePVP.h"
#include <Graphics\Graphics.h>
#include <Component\Camera\FreeCameraCom.h>
#include <Component\Camera\EventCameraCom.h>
#include <Component\Character\RegisterChara.h>
#include <Component\Camera\FPSCameraCom.h>
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
#include "Component\Audio\AudioCom.h"
#include <Component\Character\Prop\SetNodeWorldPosCom.h>
#include "Netwark/Photon/StdIO_UIListener.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include "Component\Stage\GateGimmickCom.h"
#include <StateMachine\Behaviar\InazawaCharacterState.h>
#include "Component\Sprite\Sprite.h"
#include "Component\System\GameObject.h"
#include "Component/Collsion/NodeCollsionCom.h"

void ScenePVP::Initialize()
{
    Graphics& graphics = Graphics::Instance();

#pragma region �Q�[���I�u�W�F�N�g�̐ݒ�

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

#ifdef _DEBUG
    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
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
        std::shared_ptr<RendererCom> r = stageObj->AddComponent<RendererCom>(SHADER_ID_MODEL::STAGEDEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");
        r->SetOutlineColor({ 0.000f, 0.932f, 1.000f });
        r->SetOutlineIntensity(5.5f);
        stageObj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");

        //�X�e�[�W
        StageEditorCom* stageEdit = stageObj->AddComponent<StageEditorCom>().get();
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

        RigidBodyCom* rigid = stageObj->AddComponent<RigidBodyCom>(true, RigidBodyCom::RigidType::Complex).get();
        rigid->SetUseResourcePath("Data/Model/MatuokaStage/StageJson/ColliderStage.mdl");
        rigid->SetNormalizeScale(1);
    }

    //�v���C���[
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0,0,0 });
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::INAZAWA, obj);

        auto& au = obj->AddComponent<AudioCom>();
        au->RegisterSource(AUDIOID::PLAYER_ATTACKULTBOOM, "P_ATTACK_ULT_BOOM");
        au->RegisterSource(AUDIOID::PLAYER_ATTACKULTSHOOT, "P_ATTACKULTSHOOT");
        au->RegisterSource(AUDIOID::PLAYER_CHARGE, "P_CHARGE");
        au->RegisterSource(AUDIOID::PLAYER_DAMAGE, "P_DAMAGE");
        au->RegisterSource(AUDIOID::PLAYER_DASH, "P_DASH");
        au->RegisterSource(AUDIOID::PLAYER_SHOOT, "P_SHOOT");

        //�E���g�֌WObj�ǉ�
        {
            //�A�^�b�N�n�E���g
            std::shared_ptr<GameObject> ultAttckChild = obj->AddChildObject();
            ultAttckChild->SetName("UltAttackChild");
            //�ʒu���J�����ƈꏏ�ɂ���
            ultAttckChild->transform_->SetWorldPosition({ 0, 8.0821f, 3.3050f });

            std::shared_ptr<RayColliderCom> rayCol = ultAttckChild->AddComponent<RayColliderCom>();
            rayCol->SetMyTag(COLLIDER_TAG::Player);
            rayCol->SetJudgeTag(COLLIDER_TAG::Enemy);
            rayCol->SetEnabled(false);

            //�_���[�W�����p
            std::shared_ptr<HitProcessCom> hitDamage = ultAttckChild->AddComponent<HitProcessCom>(obj);
            hitDamage->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
            hitDamage->SetValue(100);

            //�L�����N�^�[�ɓo�^
            obj->GetComponent<CharacterCom>()->SetAttackUltRayObj(ultAttckChild);
        }
        //�A�^�b�N�E���g�̃G�t�F�N�g
        {
            std::shared_ptr<GameObject> attackUltEff = obj->AddChildObject();
            attackUltEff->SetName("attackUltEFF");
            std::shared_ptr<GPUParticle> eff = attackUltEff->AddComponent<GPUParticle>(nullptr, 100);
            attackUltEff->transform_->SetRotation(obj->transform_->GetRotation());
            attackUltEff->transform_->SetWorldPosition(obj->transform_->GetWorldPosition());
            eff->Play();
        }
    }

    //�J�������v���C���[�̎q�ǂ��ɂ��Đ��䂷��
    {
        std::shared_ptr<GameObject> playerObj = GameObjectManager::Instance().Find("player");
        std::shared_ptr<GameObject> cameraPost = playerObj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();

        //pico�ʒu
        cameraPost->transform_->SetWorldPosition({ 0, 12.086f, 3.3050f });
        playerObj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());

        //�r
        {
            std::shared_ptr<GameObject> armChild = cameraPost->AddChildObject();
            armChild->SetName("armChild");
            armChild->transform_->SetScale({ 0.5f,0.5f,0.5f });
            armChild->transform_->SetLocalPosition({ 1.67f,-6.74f,0.95f });
            std::shared_ptr<RendererCom> r = armChild->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
            r->LoadModel("Data/Model/player_arm/player_arm.mdl");
            auto& anim = armChild->AddComponent<AnimationCom>();
            anim->PlayAnimation(0, false);

            //Eskill���G�t�F�N�g
            {
                std::shared_ptr<GameObject> eSkillEff = armChild->AddChildObject();
                eSkillEff->SetName("eSkillEff");
                std::shared_ptr<GPUParticle> eff = eSkillEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/InaESkill.gpuparticle", 100);
                eSkillEff->transform_->SetEulerRotation({ -7,-3,-80 });
                eSkillEff->transform_->SetLocalPosition({ -0.35f,9.84f,-0.58f });
                eff->SetLoop(false);
            }
            //�U������
            {
                std::shared_ptr<GameObject> chargeEff = armChild->AddChildObject();
                chargeEff->transform_->SetLocalPosition({ 0.98f,12.44f,6.96f });
                chargeEff->SetName("chargeEff");
                std::shared_ptr<GPUParticle> eff = chargeEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/playercharge.gpuparticle", 300);
                eff->SetLoop(false);
                //�e���ɂ����t����
                chargeEff->AddComponent<SetNodeWorldPosCom>();
            }
            //�U�����߃}�b�N�X
            {
                std::shared_ptr<GameObject> chargeMaxEff = armChild->AddChildObject();
                chargeMaxEff->transform_->SetLocalPosition({ 0.98f,12.44f,6.96f });
                chargeMaxEff->SetName("chargeMaxEff");
                std::shared_ptr<GPUParticle> eff = chargeMaxEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/playerchargeFull.gpuparticle", 300);
                eff->SetLoop(false);
                //�e���ɂ����t����
                chargeMaxEff->AddComponent<SetNodeWorldPosCom>();
            }
            //�E���g�}�Y���t���b�V��
            {
                std::shared_ptr<GameObject> attackUltMuzzleEff = armChild->AddChildObject();
                attackUltMuzzleEff->transform_->SetLocalPosition({ -3.1f,12.94f,1.69f });
                attackUltMuzzleEff->SetName("attackUltMuzzleEff");
                std::shared_ptr<GPUParticle> eff = attackUltMuzzleEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltMuzzleF.gpuparticle", 20);
                eff->SetLoop(false);
            }
            //�E���g�����ӂ��P
            {
                std::shared_ptr<GameObject> attackUltSide1 = armChild->AddChildObject();
                attackUltSide1->transform_->SetLocalPosition({ -7.915f,12.94f,1.69f });
                attackUltSide1->SetName("attackUltSide1");
                std::shared_ptr<GPUParticle> eff = attackUltSide1->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltSide.gpuparticle", 5);
                eff->SetLoop(false);
            }
            //�E���g�����ӂ��Q
            {
                std::shared_ptr<GameObject> attackUltSide2 = armChild->AddChildObject();
                attackUltSide2->transform_->SetLocalPosition({ 1.094f,12.94f,1.69f });
                attackUltSide2->SetName("attackUltSide2");
                std::shared_ptr<GPUParticle> eff = attackUltSide2->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltSide.gpuparticle", 5);
                eff->SetLoop(false);
            }
        }
    }

    //snowparticle
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("snowparticle");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/snow.gpuparticle", 10000);
    }

    //UI�Q�[���I�u�W�F�N�g����
    CreateUiObject();

#pragma endregion

#pragma region �O���t�B�b�N�n�̐ݒ�

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

    //�l�b�g�厖
    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);
}

void ScenePVP::Finalize()
{
    photonNet->close();
}

void ScenePVP::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    //�l�b�g�X�V
    photonNet->run(elapsedTime);

    //�C�x���g�J�����p
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
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

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());

    //imgui
    photonNet->ImGui();

    //�C�x���g�J�����p
    EventCameraManager::Instance().EventCameraImGui();
}

void ScenePVP::CreateUiObject()
{
    //UI
    {
        //�L�����o�X
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");

        //���e�B�N��
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

        //Ult�J�E���g
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> ultCore = canvas->AddChildObject();
            ultCore->SetName("ultCore");
            int value = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetRMaxCount();
            ultCore->AddComponent<UI_Ult_Count>(value);
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

            skillGauge->SetMaxValue(player->GetComponent<CharacterCom>()->GetESkillCoolTime());
            float* i = player->GetComponent<CharacterCom>()->GetESkillCoolTimer();
            skillGauge->SetVariableValue(i);
        }

        //Skill_E
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillGauge = SkillFrame->AddChildObject();
            skillGauge->SetName("Skill_E");
            skillGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_E.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //Skill�J�E���g
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
            skillGauge->SetMaxValue(player->GetComponent<CharacterCom>()->GetSpaceSkillCoolTime());
            float* i = player->GetComponent<CharacterCom>()->GetSpaceSkillCoolTimer();
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