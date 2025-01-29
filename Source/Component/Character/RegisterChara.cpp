#include "RegisterChara.h"
#include "Component\System\GameObject.h"
#include "Component/System/TransformCom.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Animation\AnimationCom.h"
#include "Component/Animation/AimIKCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Collsion\NodeCollsionCom.h"
#include "CharaStatusCom.h"
#include "InazawaCharacterCom.h"
#include "JankratCharacterCom.h"
#include "FarahCom.h"
#include "Component/Particle/CPUParticle.h"
#include "Component/Collsion/PushBackCom.h"
#include "Component/System/HitProcessCom.h"
#include "Component\System\SpawnCom.h"
#include "Component\Renderer\TrailCom.h"
#include "Component\Particle\GPUParticle.h"
#include <Component\Camera\FPSCameraCom.h>
#include <Component\Character\Prop\SetNodeWorldPosCom.h>
#include "Component\UI\PlayerUI.h"
#include "Component\Character\SoldierCom.h"
#include "Audio/Audio3D.h"

inline GameObj MAKE_AUDIO_3D(GameObj obj, AUDIOID3D id) {
    GameObj ultSeObj = obj->AddChildObject();
    std::shared_ptr<AudioSource3D> ultSe = ultSeObj->AddComponent<AudioSource3D>(id);
    ultSe->SetCurveDistanceScaler(70.0f);
    return ultSeObj;
}

void RegisterChara::SetCharaComponet(CHARA_LIST list, std::shared_ptr<GameObject>& obj, bool myTeam)
{
    switch (list)
    {
    case CHARA_LIST::INAZAWA:
        InazawaChara(obj, myTeam);
        break;
    case CHARA_LIST::FARAH:
        FarahCharacter(obj, myTeam);
        break;
    case CHARA_LIST::JANKRAT:
        JankratChara(obj, myTeam);
        break;
    case CHARA_LIST::SOLIDER:
        SoldireChar(obj, myTeam);
        break;
    default:
        break;
    }

    //キャラが登録された時にHP表示用のコンポーネントを用意する
    if (obj->GetComponent<Collider>()->GetMyTag() == COLLIDER_TAG::Enemy) {
        obj->AddComponent<UI_EnemyHp>();
        obj->GetComponent<UI_EnemyHp>()->Register(obj);
    }

    //自キャラの場合
    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        PlayerUIManager::Instance().BookingRegistrationUI(obj);
    }
}

void RegisterChara::ChangeChara(std::string objName, CHARA_LIST list)
{
    int teamID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetCharaData().GetTeamID();

    auto& gameobjM = GameObjectManager::Instance();
    std::weak_ptr<GameObject> p2 = gameobjM.Find(objName.c_str());
    if (!p2.lock())return;
    if (int(list) >= int(CHARA_LIST::MAX))return;

    //チーム分けをする
    int CT = p2.lock()->GetComponent<CharacterCom>()->GetNetCharaData().GetTeamID();
    bool team = false;
    if (teamID == CT)team = true;

    //削除
    gameobjM.RemoveNowTime(p2);

    //追加(変更)
    std::shared_ptr<GameObject> p = gameobjM.Create();
    p->SetName(objName.c_str());
    p->transform_->SetWorldPosition({ 0,0,0 });
    RegisterChara::Instance().SetCharaComponet(list, p, team);
    //gameobjM.CreateNowTimeSaveComponent(p);
}

//稲澤キャラ
void RegisterChara::InazawaChara(std::shared_ptr<GameObject>& obj, bool myTeam)
{
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/player_True/player1.mdl");
    r->SetDissolveThreshold(1.0f);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<AimIKCom>("spine2", nullptr);
    obj->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/player1.nodecollsion");
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<CharaStatusCom> status = obj->AddComponent<CharaStatusCom>();

    //HPの初期設定
    status->SetMaxHitPoint(200);
    status->SetHitPoint(status->GetMaxHitpoint());
    status->SetInvincibleTime(0.3f);
    std::shared_ptr<InazawaCharacterCom> c = obj->AddComponent<InazawaCharacterCom>();
    c->GetNetCharaData().SetCharaID(int(CHARA_LIST::INAZAWA));
    c->SetSkillCoolTime(CharacterCom::SkillCoolID::E, 8.0f);
    c->SetSkillCoolTime(CharacterCom::SkillCoolID::RightClick, 5.0f);
    c->SetUseSkill(USE_SKILL::E | USE_SKILL::RIGHT_CLICK);
    //音
    c->SetAudio("ATK", MAKE_AUDIO_3D(obj, AUDIOID3D::HANZO_ATK));
    c->SetAudio("DASH", MAKE_AUDIO_3D(obj, AUDIOID3D::HANZO_DASH));
    c->SetAudio("CHARGE", MAKE_AUDIO_3D(obj, AUDIOID3D::HANZO_CHARGE));
    c->SetAudio("ULT", MAKE_AUDIO_3D(obj, AUDIOID3D::HANZO_ULT));
    c->SetAudio("ULT_BOOM", MAKE_AUDIO_3D(obj, AUDIOID3D::HANZO_ULT_BOOM));

    //ボックスコライダー
    std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    if (std::strcmp(obj->GetName(), "player") == 0 || myTeam)
        box->SetMyTag(COLLIDER_TAG::Player);
    else
        box->SetMyTag(COLLIDER_TAG::Enemy);

    //押し出し処理
    auto& pushBack = obj->AddComponent<PushBackCom>();
    pushBack->SetRadius(0.5f);
    pushBack->SetWeight(1);

    //音
    std::string n = obj->GetName();
    int na = n.find("load");
    if (std::strcmp(obj->GetName(), "player") != 0) {
        if (na < 0) {
            //足音
            GameObj audio = obj->AddChildObject();
            audio->SetName("footEmitter");
            auto& au = audio->AddComponent<AudioSource3D>(AUDIOID3D::PLAYER_WAKL);
            au->SetVolume(10);
            au->AudioPlay();
        }
    }

    //煙のエフェクト
    {
        std::shared_ptr<GameObject> smoke = obj->AddChildObject();
        smoke->SetName("smokeeffect");
        std::shared_ptr<CPUParticle> smokeeffct = smoke->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/smoke.cpuparticle", 100);
        smokeeffct->SetActive(false);
    }

    //ウルト関係Obj追加
    {
        //アタック系ウルト
        std::shared_ptr<GameObject> ultAttckChild = obj->AddChildObject();
        ultAttckChild->SetName("UltAttackChild");
        //位置をカメラと一緒にする
        ultAttckChild->transform_->SetWorldPosition({ 0, 8.0821f, 3.3050f });

        std::shared_ptr<RayColliderCom> rayCol = ultAttckChild->AddComponent<RayColliderCom>();

        if (std::strcmp(obj->GetName(), "player") == 0 || myTeam)
        {
            rayCol->SetMyTag(COLLIDER_TAG::Player);
            rayCol->SetJudgeTag(COLLIDER_TAG::Enemy | COLLIDER_TAG::UnderStand);
        }
        else
        {
            rayCol->SetMyTag(COLLIDER_TAG::Enemy);
            rayCol->SetJudgeTag(COLLIDER_TAG::Player | COLLIDER_TAG::UnderStand);
        }

        rayCol->SetEnabled(false);

        //ダメージ処理用
        std::shared_ptr<HitProcessCom> hitDamage = ultAttckChild->AddComponent<HitProcessCom>(obj);
        hitDamage->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
        hitDamage->SetValue(100);

        //キャラクターに登録
        obj->GetComponent<InazawaCharacterCom>()->SetAttackUltRayObj(ultAttckChild);
    }
    //アタックウルトのエフェクト
    {
        std::shared_ptr<GameObject> attackUltEff = obj->AddChildObject();
        attackUltEff->SetName("attackUltEFF");
        std::shared_ptr<GPUParticle> eff = attackUltEff->AddComponent<GPUParticle>(nullptr, 100);
        attackUltEff->transform_->SetRotation(obj->transform_->GetRotation());
        attackUltEff->transform_->SetWorldPosition(obj->transform_->GetWorldPosition());
        eff->Play();
    }

    //ネットでは見える化
    if (std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetDissolveThreshold(0.0f);
    }

    if (!myTeam && std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetOutlineColor({ 1.0f, 0.0f, 0.0f });
        r->SetOutlineIntensity(11.5f);
    }

    if (myTeam && std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetOutlineColor({ 0.0f, 0.0f, 1.0f });
        r->SetOutlineIntensity(11.5f);
    }

    //腕とカメラの処理カメラをプレイヤーの子どもにして制御する
    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        std::shared_ptr<GameObject> cameraPost = obj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom> fpscamera = cameraPost->AddComponent<FPSCameraCom>();
        fpscamera->ActiveCameraChange();

        //カメラ位置
        cameraPost->transform_->SetWorldPosition({ 0, 12.086f, 3.3050f });
        obj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());

        //腕
        {
            std::shared_ptr<GameObject> armChild = cameraPost->AddChildObject();
            armChild->SetName("armChild");
            armChild->transform_->SetScale({ 0.5f,0.5f,0.5f });
            armChild->transform_->SetLocalPosition({ 1.67f,-6.74f,0.95f });
            std::shared_ptr<RendererCom> r = armChild->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, false, false);
            r->LoadModel("Data/Model/player_arm/player_arm.mdl");
            auto& anim = armChild->AddComponent<AnimationCom>();

            //Eskill中エフェクト
            {
                std::shared_ptr<GameObject> eSkillEff = armChild->AddChildObject();
                eSkillEff->SetName("eSkillEff");
                std::shared_ptr<GPUParticle> eff = eSkillEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/InaESkill.gpuparticle", 100);
                eSkillEff->transform_->SetEulerRotation({ -7,-3,-80 });
                eSkillEff->transform_->SetLocalPosition({ -0.35f,9.84f,-0.58f });
                eff->SetLoop(false);
            }
            //攻撃ため
            {
                std::shared_ptr<GameObject> chargeEff = armChild->AddChildObject();
                chargeEff->transform_->SetLocalPosition({ 0.98f,12.44f,6.96f });
                chargeEff->SetName("chargeEff");
                std::shared_ptr<GPUParticle> eff = chargeEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/playercharge.gpuparticle", 300);
                eff->SetLoop(false);
                //銃口にくっ付ける
                chargeEff->AddComponent<SetNodeWorldPosCom>();
            }
            //攻撃ためマックス
            {
                std::shared_ptr<GameObject> chargeMaxEff = armChild->AddChildObject();
                chargeMaxEff->transform_->SetLocalPosition({ 0.98f,12.44f,6.96f });
                chargeMaxEff->SetName("chargeMaxEff");
                std::shared_ptr<GPUParticle> eff = chargeMaxEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/playerchargeFull.gpuparticle", 300);
                eff->SetLoop(false);
                //銃口にくっ付ける
                chargeMaxEff->AddComponent<SetNodeWorldPosCom>();
            }
            //ウルトマズルフラッシュ
            {
                std::shared_ptr<GameObject> attackUltMuzzleEff = armChild->AddChildObject();
                attackUltMuzzleEff->transform_->SetLocalPosition({ -3.1f,12.94f,1.69f });
                attackUltMuzzleEff->SetName("attackUltMuzzleEff");
                std::shared_ptr<GPUParticle> eff = attackUltMuzzleEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltMuzzleF.gpuparticle", 20);
                eff->SetLoop(false);
            }
            //ウルト中えふぇ１
            {
                std::shared_ptr<GameObject> attackUltSide1 = armChild->AddChildObject();
                attackUltSide1->transform_->SetLocalPosition({ -7.915f,12.94f,1.69f });
                attackUltSide1->SetName("attackUltSide1");
                std::shared_ptr<GPUParticle> eff = attackUltSide1->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltSide.gpuparticle", 5);
                eff->SetLoop(false);
            }
            //ウルト中えふぇ２
            {
                std::shared_ptr<GameObject> attackUltSide2 = armChild->AddChildObject();
                attackUltSide2->transform_->SetLocalPosition({ 1.094f,12.94f,1.69f });
                attackUltSide2->SetName("attackUltSide2");
                std::shared_ptr<GPUParticle> eff = attackUltSide2->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltSide.gpuparticle", 5);
                eff->SetLoop(false);
            }
            //スタンエフェクト
            {
                std::shared_ptr<GameObject> stanEff = armChild->AddChildObject();
                stanEff->SetName("stanEff");
                stanEff->transform_->SetLocalPosition({ -3.1f,12.94f,1.69f });
                auto& gpuP = stanEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/stanDamageEff.gpuparticle", 250);
                gpuP->SetLoop(false);
            }
        }
    }

    //敵の場合はピン用の当たりを設定
    if (std::strcmp(obj->GetName(), "player") != 0 && !myTeam)
    {
        std::shared_ptr<GameObject> pin = obj->AddChildObject();
        pin->SetName("pinCol");

        pin->transform_->SetLocalPosition({ 0,7,0 });

        //ノードコリジョン用モデル
        std::shared_ptr<RendererCom> r = pin->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, false, false);
        r->LoadModel("Data/Model/player_arm/player_arm.mdl");
        r->SetDissolveThreshold(1);

        std::shared_ptr<SphereColliderCom> Sphere = pin->AddComponent<SphereColliderCom>();
        Sphere->SetRadius(2.5f);
        Sphere->SetMyTag(COLLIDER_TAG::pinCharacter);

        pin->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/pinChara.nodecollsion");
    }

}

//ファラ
void RegisterChara::FarahCharacter(std::shared_ptr<GameObject>& obj, bool myTeam)
{
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/player_True/player2.mdl");
    r->SetDissolveThreshold(1.0f);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<AimIKCom>("spine2", nullptr);
    obj->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/player2.nodecollsion");
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<CharaStatusCom> status = obj->AddComponent<CharaStatusCom>();

    //HPの初期設定
    status->SetMaxHitPoint(200);
    status->SetHitPoint(status->GetMaxHitpoint());
    status->SetInvincibleTime(0.3f);
    std::shared_ptr<FarahCom> c = obj->AddComponent<FarahCom>();
    c->GetNetCharaData().SetCharaID(int(CHARA_LIST::FARAH));
    c->SetSkillCoolTime(CharacterCom::SkillCoolID::E, 8.0f);
    c->SetSkillCoolTime(CharacterCom::SkillCoolID::RightClick, 6.0f);
    c->SetUseSkill(USE_SKILL::E | USE_SKILL::RIGHT_CLICK);
    c->SetAudio("JET", MAKE_AUDIO_3D(obj, AUDIOID3D::FARAH_JET));

    //ボックスコライダー
    std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    if (std::strcmp(obj->GetName(), "player") == 0 || myTeam)
        box->SetMyTag(COLLIDER_TAG::Player);
    else
        box->SetMyTag(COLLIDER_TAG::Enemy);

    //音
    std::string n = obj->GetName();
    int na = n.find("load");
    if (std::strcmp(obj->GetName(), "player") != 0) {
        if (na < 0) {
            //足音
            GameObj audio = obj->AddChildObject();
            audio->SetName("footEmitter");
            auto& au = audio->AddComponent<AudioSource3D>(AUDIOID3D::PLAYER_WAKL);
            au->SetVolume(10);
            au->AudioPlay();
        }
    }

    //押し出し処理
    auto& pushBack = obj->AddComponent<PushBackCom>();
    pushBack->SetRadius(0.5f);
    pushBack->SetWeight(1);

    //煙のエフェクト
    {
        std::shared_ptr<GameObject> smoke = obj->AddChildObject();
        smoke->SetName("smokeeffect");
        std::shared_ptr<CPUParticle> smokeeffct = smoke->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/smoke.cpuparticle", 100);
        smokeeffct->SetActive(false);
    }

    //ult
    {
        std::shared_ptr<GameObject>ultobject = obj->AddChildObject();
        ultobject->SetName("UltObject");
        ultobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/farah_UltSkill.gpuparticle", 2000);
    }

    //ブーストエフェクト1、２
    {
        std::shared_ptr<GameObject>boost1 = obj->AddChildObject();
        boost1->SetName("Boost1");
        std::shared_ptr<GPUParticle>p = boost1->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/farah_Jump.gpuparticle", 500);
        p->SetLoop(false);
    }
    {
        std::shared_ptr<GameObject>boost2 = obj->AddChildObject();
        boost2->SetName("Boost2");
        std::shared_ptr<GPUParticle>p = boost2->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/farah_Jump.gpuparticle", 500);
        p->SetLoop(false);
    }

    //ネットでは見える化
    if (std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetDissolveThreshold(0.0f);
    }

    if (!myTeam && std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetOutlineColor({ 1.0f, 0.0f, 0.0f });
        r->SetOutlineIntensity(11.5f);
    }
    if (myTeam && std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetOutlineColor({ 0.0f, 0.0f, 1.0f });
        r->SetOutlineIntensity(11.5f);
    }

    //腕とカメラの処理カメラをプレイヤーの子どもにして制御する
    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        std::shared_ptr<GameObject> cameraPost = obj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();
        fpscamera->ActiveCameraChange();

        //カメラ位置
        cameraPost->transform_->SetWorldPosition({ 0, 12.086f, 3.3050f });
        obj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());

        //腕
        {
            std::shared_ptr<GameObject> armChild = cameraPost->AddChildObject();
            armChild->SetName("armChild");
            armChild->transform_->SetScale({ 0.5f,0.5f,0.5f });
            armChild->transform_->SetLocalPosition({ 1.67f,-6.74f,0.95f });
            std::shared_ptr<RendererCom> r = armChild->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, false, false);
            r->LoadModel("Data/Model/player_arm/player_arm_2.mdl");
            armChild->AddComponent<AnimationCom>();

            //マゼルフラッシュ
            std::shared_ptr<GameObject>particleobj = armChild->AddChildObject();
            particleobj->SetName("muzzleflash");
            std::shared_ptr<CPUParticle>cpuparticle = particleobj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/player_muzzleflash.cpuparticle", 10);
            cpuparticle->SetActive(false);

            //スタンエフェクト
            {
                std::shared_ptr<GameObject> stanEff = armChild->AddChildObject();
                stanEff->SetName("stanEff");
                stanEff->transform_->SetLocalPosition({ -3.1f,12.94f,1.69f });
                auto& gpuP = stanEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/stanDamageEff.gpuparticle", 250);
                gpuP->SetLoop(false);
            }
        }
    }

    //敵の場合はピン用の当たりを設定
    if (std::strcmp(obj->GetName(), "player") != 0 && !myTeam)
    {
        std::shared_ptr<GameObject> pin = obj->AddChildObject();
        pin->SetName("pinCol");

        pin->transform_->SetLocalPosition({ 0,7,0 });

        //ノードコリジョン用モデル
        std::shared_ptr<RendererCom> r = pin->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, false, false);
        r->LoadModel("Data/Model/player_arm/player_arm.mdl");
        r->SetDissolveThreshold(1);

        std::shared_ptr<SphereColliderCom> Sphere = pin->AddComponent<SphereColliderCom>();
        Sphere->SetRadius(2.5f);
        Sphere->SetMyTag(COLLIDER_TAG::pinCharacter);

        pin->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/pinChara.nodecollsion");
    }
}

void RegisterChara::JankratChara(std::shared_ptr<GameObject>& obj, bool myTeam)
{
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/player_True/player3.mdl");
    r->SetDissolveThreshold(1.0f);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/player3.nodecollsion");
    obj->AddComponent<AimIKCom>("spine2", nullptr);
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<CharaStatusCom> status = obj->AddComponent<CharaStatusCom>();
    std::shared_ptr<JankratCharacterCom> charaCom = obj->AddComponent<JankratCharacterCom>();
    charaCom->GetNetCharaData().SetCharaID(int(CHARA_LIST::JANKRAT));
    charaCom->SetMaxBulletNum(15);
    charaCom->SetCurrentBulletNum(15);
    charaCom->SetSkillCoolTime(CharacterCom::SkillCoolID::E, 3.5f);
    charaCom->SetSkillCoolTime(CharacterCom::SkillCoolID::RightClick, 0.1f);
    charaCom->SetUseSkill(USE_SKILL::E | USE_SKILL::RIGHT_CLICK);

    //HPの初期設定
    status->SetMaxHitPoint(200);
    status->SetHitPoint(status->GetMaxHitpoint());
    status->SetInvincibleTime(0.3f);

    //ボックスコライダー
    std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    if (std::strcmp(obj->GetName(), "player") == 0 || myTeam)
        box->SetMyTag(COLLIDER_TAG::Player);
    else
        box->SetMyTag(COLLIDER_TAG::Enemy);

    //音
    std::string n = obj->GetName();
    int na = n.find("load");
    if (std::strcmp(obj->GetName(), "player") != 0) {
        if (na < 0) {
            //足音
            GameObj audio = obj->AddChildObject();
            audio->SetName("footEmitter");
            auto& au = audio->AddComponent<AudioSource3D>(AUDIOID3D::PLAYER_WAKL);
            au->SetVolume(10);
            au->AudioPlay();
        }
    }

    //SE登録
    charaCom->SetAudio("ATK1", MAKE_AUDIO_3D(obj, AUDIOID3D::JANKRA_ATK1));
    charaCom->SetAudio("ATK2", MAKE_AUDIO_3D(obj, AUDIOID3D::JANKRA_ATK2));
    charaCom->SetAudio("ULT1", MAKE_AUDIO_3D(obj, AUDIOID3D::JANKRA_ULT));
    charaCom->SetAudio("ULT2", MAKE_AUDIO_3D(obj, AUDIOID3D::JANKRA_ULT2));
    charaCom->SetAudio("MINE", MAKE_AUDIO_3D(obj, AUDIOID3D::JANKRA_MINE));

    //押し出し処理
    auto& pushBack = obj->AddComponent<PushBackCom>();
    pushBack->SetRadius(0.5f);
    pushBack->SetWeight(1);

    //ネットでは見える化
    if (std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetDissolveThreshold(0.0f);
    }

    if (!myTeam && std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetOutlineColor({ 1.0f, 0.0f, 0.0f });
        r->SetOutlineIntensity(11.5f);
    }
    if (myTeam && std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetOutlineColor({ 0.0f, 0.0f, 1.0f });
        r->SetOutlineIntensity(11.5f);
    }

    //煙のエフェクト
    {
        std::shared_ptr<GameObject> smoke = obj->AddChildObject();
        smoke->SetName("smokeeffect");
        std::shared_ptr<CPUParticle> smokeeffct = smoke->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/smoke.cpuparticle", 100);
        smokeeffct->SetActive(false);
    }

    //ブーストエフェクト1、２
    {
        std::shared_ptr<GameObject>boost1 = obj->AddChildObject();
        boost1->SetName("Boost1");
        std::shared_ptr<GPUParticle>p = boost1->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/santrat_body.gpuparticle", 1000);
    }
    {
        std::shared_ptr<GameObject>boost2 = obj->AddChildObject();
        boost2->SetName("Boost2");
        std::shared_ptr<GPUParticle>p = boost2->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/santrat_body.gpuparticle", 1000);
    }

    //自分かネットのプレイヤーで
    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        //腕とカメラの処理カメラをプレイヤーの子どもにして制御する
        std::shared_ptr<GameObject> cameraPost = obj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();
        fpscamera->ActiveCameraChange();

        //カメラ位置
        cameraPost->transform_->SetLocalPosition({ 0, 12.086f, 3.3050f });
        obj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());

        //腕
        {
            std::shared_ptr<GameObject> armChild = cameraPost->AddChildObject();
            armChild->SetName("armChild");
            armChild->transform_->SetScale({ 0.5f,0.5f,0.5f });
            armChild->transform_->SetLocalPosition({ 1.67f,-6.74f,0.95f });
            std::shared_ptr<RendererCom> r = armChild->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, false, false);
            r->LoadModel("Data/Model/player_arm/player_arm_3.mdl");
            armChild->AddComponent<AnimationCom>();

            //マゼルフラッシュ
            std::shared_ptr<GameObject>particleobj = armChild->AddChildObject();
            particleobj->SetName("muzzleflash");
            std::shared_ptr<CPUParticle>cpuparticle = particleobj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/player_muzzleflash.cpuparticle", 10);
            cpuparticle->SetActive(false);

            //スタンエフェクト
            {
                std::shared_ptr<GameObject> stanEff = armChild->AddChildObject();
                stanEff->SetName("stanEff");
                stanEff->transform_->SetLocalPosition({ -3.1f,12.94f,1.69f });
                auto& gpuP = stanEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/stanDamageEff.gpuparticle", 250);
                gpuP->SetLoop(false);
            }
        }
    }

    //敵の場合はピン用の当たりを設定
    if (std::strcmp(obj->GetName(), "player") != 0 && !myTeam)
    {
        std::shared_ptr<GameObject> pin = obj->AddChildObject();
        pin->SetName("pinCol");

        pin->transform_->SetLocalPosition({ 0,7,0 });

        //ノードコリジョン用モデル
        std::shared_ptr<RendererCom> r = pin->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, false, false);
        r->LoadModel("Data/Model/player_arm/player_arm.mdl");
        r->SetDissolveThreshold(1);

        std::shared_ptr<SphereColliderCom> Sphere = pin->AddComponent<SphereColliderCom>();
        Sphere->SetRadius(2.5f);
        Sphere->SetMyTag(COLLIDER_TAG::pinCharacter);

        pin->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/pinChara.nodecollsion");
    }
}

//ソルジャー
void RegisterChara::SoldireChar(std::shared_ptr<GameObject>& obj, bool myTeam)
{
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/player_True/player4.mdl");
    r->SetDissolveThreshold(1.0f);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<AimIKCom>("spine2", nullptr);
    obj->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/player4.nodecollsion");
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<CharaStatusCom> status = obj->AddComponent<CharaStatusCom>();

    //HPの初期設定
    status->SetMaxHitPoint(200);
    status->SetHitPoint(status->GetMaxHitpoint());
    status->SetInvincibleTime(0.3f);
    std::shared_ptr<SoldierCom> c = obj->AddComponent<SoldierCom>();
    c->GetNetCharaData().SetCharaID(int(CHARA_LIST::SOLIDER));
    c->SetSkillCoolTime(CharacterCom::SkillCoolID::E, 8.0f);
    c->SetSkillCoolTime(CharacterCom::SkillCoolID::RightClick, 6.0f);
    c->SetUseSkill(USE_SKILL::E | USE_SKILL::RIGHT_CLICK);
    c->SetCurrentBulletNum(20);
    c->SetMaxBulletNum(20);

    c->SetAudio("ATK", MAKE_AUDIO_3D(obj, AUDIOID3D::SOLDIER_ATK));
    c->SetAudio("SKILL1", MAKE_AUDIO_3D(obj, AUDIOID3D::SOLDIER_SKILL1));
    c->SetAudio("SKILL2", MAKE_AUDIO_3D(obj, AUDIOID3D::SOLDIER_SKILL2));
    c->SetAudio("ULT", MAKE_AUDIO_3D(obj, AUDIOID3D::SOLDIER_ULT));

    //ボックスコライダー
    std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    if (std::strcmp(obj->GetName(), "player") == 0 || myTeam)
        box->SetMyTag(COLLIDER_TAG::Player);
    else
        box->SetMyTag(COLLIDER_TAG::Enemy);

    //音
    std::string n = obj->GetName();
    int na = n.find("load");
    if (std::strcmp(obj->GetName(), "player") != 0) {
        if (na < 0) {
            //足音
            GameObj audio = obj->AddChildObject();
            audio->SetName("footEmitter");
            auto& au = audio->AddComponent<AudioSource3D>(AUDIOID3D::PLAYER_WAKL);
            au->SetVolume(10);
            au->AudioPlay();
        }
    }

    //押し出し処理
    auto& pushBack = obj->AddComponent<PushBackCom>();
    pushBack->SetRadius(0.5f);
    pushBack->SetWeight(1);

    //煙のエフェクト
    {
        std::shared_ptr<GameObject> smoke = obj->AddChildObject();
        smoke->SetName("smokeeffect");
        std::shared_ptr<CPUParticle> smokeeffct = smoke->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/smoke.cpuparticle", 100);
        smokeeffct->SetActive(false);
    }

    //ult
    {
        std::shared_ptr<GameObject>ultobject = obj->AddChildObject();
        ultobject->SetName("UltObject");
        ultobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/solder_ult_aura.gpuparticle", 2000);
        auto& spawn = ultobject->AddComponent<SpawnCom>("Data/SerializeData/SpawnData/soldier_ult.spawn");
        spawn->SetParentObjectKun(obj);
    }

    //ブーストエフェクト1、２
    {
        std::shared_ptr<GameObject>boost1 = obj->AddChildObject();
        boost1->SetName("Boost1");
        std::shared_ptr<GPUParticle>p = boost1->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/matya_body.gpuparticle", 2000);
        boost1->transform_->SetWorldPosition({ 1.180f,10.533f,-1.194f });
    }
    {
        std::shared_ptr<GameObject>boost2 = obj->AddChildObject();
        boost2->SetName("Boost2");
        std::shared_ptr<GPUParticle>p = boost2->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/matya_body.gpuparticle", 2000);
        boost2->transform_->SetWorldPosition({ -0.105f, 10.505f, -1.080f });
    }

    //ウルト時のスタンを纏うゲームオブジェクト
    {
        std::shared_ptr<GameObject>ultkun = obj->AddChildObject();
        ultkun->SetName("UltStanObj");
        std::shared_ptr<CapsuleColliderCom> collider = ultkun->AddComponent<CapsuleColliderCom>();

        if (std::strcmp(obj->GetName(), "player") == 0 || myTeam)
        {
            collider->SetMyTag(COLLIDER_TAG::Player);
            collider->SetJudgeTag(COLLIDER_TAG::Enemy | COLLIDER_TAG::UnderStand);
        }
        else
        {
            collider->SetMyTag(COLLIDER_TAG::Enemy);
            collider->SetJudgeTag(COLLIDER_TAG::Player | COLLIDER_TAG::UnderStand);
        }

        collider->SetRadius(0.53f);
        collider->SetPosition1({ 0.0f,2.6f,0.0f });
        ultkun->SetEnabled(false);

        //スタン処理用
        std::shared_ptr<HitProcessCom> hitstan = ultkun->AddComponent<HitProcessCom>(obj);
        hitstan->SetHitType(HitProcessCom::HIT_TYPE::STAN);
        hitstan->SetValue(1.0f);
        hitstan->SetHitInterval(0.5f);
    }

    //ヒットスキャン
    {
        std::shared_ptr<GameObject> ultAttckChild = obj->AddChildObject();
        ultAttckChild->SetName("mainattack");

        //位置をカメラと一緒にする
        ultAttckChild->transform_->SetWorldPosition({ 0, 12.086f, 3.3050f });

        //コライダーセット
        std::shared_ptr<RayColliderCom> rayCol = ultAttckChild->AddComponent<RayColliderCom>();
        rayCol->SetEnabled(false);
        rayCol->SetMyTag(COLLIDER_TAG::Bullet);
        if (std::strcmp(obj->GetName(), "player") == 0 || myTeam)
            rayCol->SetJudgeTag(COLLIDER_TAG::Enemy | COLLIDER_TAG::EnemyBullet | COLLIDER_TAG::UnderStand);
        else
            rayCol->SetJudgeTag(COLLIDER_TAG::Player | COLLIDER_TAG::UnderStand);

        //ダメージ処理用
        std::shared_ptr<HitProcessCom> hitDamage = ultAttckChild->AddComponent<HitProcessCom>(obj);
        hitDamage->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
        hitDamage->SetValue(15);

        //キャラクターに登録
        obj->GetComponent<SoldierCom>()->SetAttackRayObj(ultAttckChild);
    }

    //ネットでは見える化
    if (std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetDissolveThreshold(0.0f);
    }

    if (!myTeam && std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetOutlineColor({ 1.0f, 0.0f, 0.0f });
        r->SetOutlineIntensity(11.5f);
    }
    if (myTeam && std::strcmp(obj->GetName(), "player") != 0)
    {
        r->SetOutlineColor({ 0.0f, 0.0f, 1.0f });
        r->SetOutlineIntensity(11.5f);
    }

    //腕とカメラの処理カメラをプレイヤーの子どもにして制御する
    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        std::shared_ptr<GameObject> cameraPost = obj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();
        fpscamera->ActiveCameraChange();

        //カメラ位置
        cameraPost->transform_->SetWorldPosition({ 0, 12.086f, 3.3050f });
        obj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());

        //腕
        {
            std::shared_ptr<GameObject> armChild = cameraPost->AddChildObject();
            armChild->SetName("armChild");
            armChild->transform_->SetScale({ 0.5f,0.5f,0.5f });
            armChild->transform_->SetLocalPosition({ 1.67f,-6.74f,0.95f });
            std::shared_ptr<RendererCom> r = armChild->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, false, false);
            r->LoadModel("Data/Model/player_arm/player_arm_4.mdl");
            armChild->AddComponent<AnimationCom>();

            //マゼルフラッシュ
            std::shared_ptr<GameObject>particleobj = armChild->AddChildObject();
            particleobj->SetName("muzzleflash");
            std::shared_ptr<CPUParticle>cpuparticle = particleobj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/player_muzzleflash.cpuparticle", 10);
            cpuparticle->SetActive(false);

            //スタンエフェクト
            {
                std::shared_ptr<GameObject> stanEff = armChild->AddChildObject();
                stanEff->SetName("stanEff");
                stanEff->transform_->SetLocalPosition({ -3.1f,12.94f,1.69f });
                auto& gpuP = stanEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/stanDamageEff.gpuparticle", 250);
                gpuP->SetLoop(false);
            }
        }
    }

    //着火
    {
        std::shared_ptr<GameObject> beem = obj->AddChildObject();
        beem->SetName("beem_fire");
        beem->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/soldier_mainattack_fire.gpuparticle", 300);
    }

    //敵の場合はピン用の当たりを設定
    if (std::strcmp(obj->GetName(), "player") != 0 && !myTeam)
    {
        std::shared_ptr<GameObject> pin = obj->AddChildObject();
        pin->SetName("pinCol");

        pin->transform_->SetLocalPosition({ 0,7,0 });

        //ノードコリジョン用モデル
        std::shared_ptr<RendererCom> r = pin->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, false, false);
        r->LoadModel("Data/Model/player_arm/player_arm.mdl");
        r->SetDissolveThreshold(1);

        std::shared_ptr<SphereColliderCom> Sphere = pin->AddComponent<SphereColliderCom>();
        Sphere->SetRadius(2.5f);
        Sphere->SetMyTag(COLLIDER_TAG::pinCharacter);

        pin->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/pinChara.nodecollsion");
    }
}