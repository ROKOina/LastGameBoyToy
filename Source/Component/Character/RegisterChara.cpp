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
#include "HaveAllAttackCharacter.h"
#include "JankratCharacterCom.h"
#include "FarahCom.h"
#include "Component/Particle/CPUParticle.h"
#include "Component/Collsion/PushBackCom.h"
#include "Component/System/HitProcessCom.h"
#include "Component\System\SpawnCom.h"
#include "Component\Renderer\TrailCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Audio\AudioCom.h"
#include <Component\Camera\FPSCameraCom.h>
#include <Component\Character\Prop\SetNodeWorldPosCom.h>
#include "Component\UI\PlayerUI.h"

void RegisterChara::SetCharaComponet(CHARA_LIST list, std::shared_ptr<GameObject>& obj)
{
    switch (list)
    {
    case RegisterChara::CHARA_LIST::INAZAWA:
        InazawaChara(obj);
        break;
    case RegisterChara::CHARA_LIST::HAVE_ALL_ATTACK:
        HaveAllAttackChara(obj);
        break;
    case RegisterChara::CHARA_LIST::FARAH:
        FarahCharacter(obj);
        break;
    case RegisterChara::CHARA_LIST::JANKRAT:
        JankratChara(obj);
        break;
    default:
        break;
    }

    //���L�����̏ꍇ
    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        PlayerUIManager::Instance().BookingRegistrationUI(obj);
        
    }

}

//imgui
void RegisterChara::ImGui()
{
}

//���V�L����
void RegisterChara::InazawaChara(std::shared_ptr<GameObject>& obj)
{
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/player_True/player.mdl");
    r->SetDissolveThreshold(1.0f);
    obj->AddComponent<AimIKCom>("spine2", nullptr);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/player.nodecollsion");
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<CharaStatusCom> status = obj->AddComponent<CharaStatusCom>();

    //HP�̏����ݒ�
    status->SetMaxHitPoint(200);
    status->SetHitPoint(status->GetMaxHitpoint());
    status->SetInvincibleTime(0.3f);
    std::shared_ptr<InazawaCharacterCom> c = obj->AddComponent<InazawaCharacterCom>();
    c->SetCharaID(int(CHARA_LIST::INAZAWA));
    c->SetSkillCoolTime(CharacterCom::SkillCoolID::E, 8.0f);
    c->SetSkillCoolTime(CharacterCom::SkillCoolID::LeftClick, 5.0f);
    //c->SetUseSkill(USE_SKILL::E );
    c->SetUseSkill(USE_SKILL::E | USE_SKILL::LEFT_CLICK);

    //�{�b�N�X�R���C�_�[
    std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    if (std::strcmp(obj->GetName(), "player") == 0)
        box->SetMyTag(COLLIDER_TAG::Player);
    else
        box->SetMyTag(COLLIDER_TAG::Enemy);

    //�����o������
    auto& pushBack = obj->AddComponent<PushBackCom>();
    pushBack->SetRadius(0.5f);
    pushBack->SetWeight(1);

    //���̃G�t�F�N�g
    {
        std::shared_ptr<GameObject> smoke = obj->AddChildObject();
        smoke->SetName("smokeeffect");
        std::shared_ptr<CPUParticle> smokeeffct = smoke->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/smoke.cpuparticle", 100);
        smokeeffct->SetActive(false);
    }

    //SE
    {
        auto& au = obj->AddComponent<AudioCom>();
        au->RegisterSource(AUDIOID::PLAYER_ATTACKULTBOOM, "P_ATTACK_ULT_BOOM");
        au->RegisterSource(AUDIOID::PLAYER_ATTACKULTSHOOT, "P_ATTACKULTSHOOT");
        au->RegisterSource(AUDIOID::PLAYER_CHARGE, "P_CHARGE");
        au->RegisterSource(AUDIOID::PLAYER_DAMAGE, "P_DAMAGE");
        au->RegisterSource(AUDIOID::PLAYER_DASH, "P_DASH");
        au->RegisterSource(AUDIOID::PLAYER_SHOOT, "P_SHOOT");
    }

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
        obj->GetComponent<InazawaCharacterCom>()->SetAttackUltRayObj(ultAttckChild);
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

    //�r�ƃJ�����̏����J�������v���C���[�̎q�ǂ��ɂ��Đ��䂷��
    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        std::shared_ptr<GameObject> cameraPost = obj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom> fpscamera = cameraPost->AddComponent<FPSCameraCom>();
        //fpscamera->ActiveCameraChange();

        //�J�����ʒu
        cameraPost->transform_->SetWorldPosition({ 0, 12.086f, 3.3050f });
        obj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());

        //�r
        {
            std::shared_ptr<GameObject> armChild = cameraPost->AddChildObject();
            armChild->SetName("armChild");
            armChild->transform_->SetScale({ 0.5f,0.5f,0.5f });
            armChild->transform_->SetLocalPosition({ 1.67f,-6.74f,0.95f });
            std::shared_ptr<RendererCom> r = armChild->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
            r->LoadModel("Data/Model/player_arm/player_arm.mdl");
            auto& anim = armChild->AddComponent<AnimationCom>();

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
}

//�S�Ă����˔�������΂����
void RegisterChara::HaveAllAttackChara(std::shared_ptr<GameObject>& obj)
{
    obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/pico/pico.mdl");
    obj->AddComponent<AimIKCom>("Spine", nullptr);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>("Data/Model/pico/pico.nodecollsion");
    obj->AddComponent<CharaStatusCom>();
    obj->AddComponent<MovementCom>();
    std::shared_ptr<HaveAllAttackCharaCom> c = obj->AddComponent<HaveAllAttackCharaCom>();
    c->SetCharaID(int(CHARA_LIST::HAVE_ALL_ATTACK));

    //���̃G�t�F�N�g
    {
        std::shared_ptr<GameObject> smoke = obj->AddChildObject();
        smoke->SetName("smokeeffect");
        std::shared_ptr<CPUParticle> smokeeffct = smoke->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/smoke.cpuparticle", 100);
        smokeeffct->SetActive(false);
    }

    //std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    //box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    //box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    //if (std::strcmp(obj->GetName(), "player") == 0)
    //    box->SetMyTag(COLLIDER_TAG::Player);
    //else
    //    box->SetMyTag(COLLIDER_TAG::Enemy);

    std::shared_ptr<CapsuleColliderCom> ca = obj->AddComponent<CapsuleColliderCom>();

    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        ca->SetMyTag(COLLIDER_TAG::Player);
        ca->SetJudgeTag(COLLIDER_TAG::Enemy);
    }
    else
    {
        ca->SetMyTag(COLLIDER_TAG::Enemy);
        ca->SetJudgeTag(COLLIDER_TAG::Player);
    }

    //std::shared_ptr<SphereColliderCom> sphere= obj->AddComponent<SphereColliderCom>();
    //sphere->SetRadius(2);
    //sphere->SetMyTag(COLLIDER_TAG::Player);
    //sphere->SetJudgeTag(COLLIDER_TAG::Enemy);

    //�U�����C�L���X�g
    {
        std::shared_ptr<GameObject> rayChild = obj->AddChildObject();
        rayChild->SetName("rayObj");

        rayChild->transform_->SetWorldPosition({ 0, 80.821f, 33.050f });

        std::shared_ptr<RayColliderCom> rayCol = rayChild->AddComponent<RayColliderCom>();
        if (std::strcmp(obj->GetName(), "player") == 0)
        {
            rayCol->SetMyTag(COLLIDER_TAG::Player);
            rayCol->SetJudgeTag(COLLIDER_TAG::Enemy);
        }
        else
        {
            rayCol->SetMyTag(COLLIDER_TAG::Enemy);
            rayCol->SetJudgeTag(COLLIDER_TAG::Player);
        }
        rayCol->SetEnabled(false);

        //�_���[�W�����p
        std::shared_ptr<HitProcessCom> hitDamage = rayChild->AddComponent<HitProcessCom>(obj);
        hitDamage->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
    }

    //�񕜃J�v�Z��
    {
        std::shared_ptr<GameObject> cupsuleChild = obj->AddChildObject();
        cupsuleChild->SetName("capsuleObj");

        cupsuleChild->transform_->SetWorldPosition({ 0, 80.821f, 33.050f });

        std::shared_ptr<CapsuleColliderCom> capsuleCol = cupsuleChild->AddComponent<CapsuleColliderCom>();
        if (std::strcmp(obj->GetName(), "player") == 0)
        {
            capsuleCol->SetMyTag(COLLIDER_TAG::Player);
            capsuleCol->SetJudgeTag(COLLIDER_TAG::Enemy);
        }
        else
        {
            capsuleCol->SetMyTag(COLLIDER_TAG::Enemy);
            capsuleCol->SetJudgeTag(COLLIDER_TAG::Player);
        }
        capsuleCol->SetEnabled(false);

        //�q�[�������p
        std::shared_ptr<HitProcessCom> hitHeal = cupsuleChild->AddComponent<HitProcessCom>(obj);
        hitHeal->SetHitType(HitProcessCom::HIT_TYPE::HEAL);
        hitHeal->SetValue(2);
    }
}

//�t�@��
void RegisterChara::FarahCharacter(std::shared_ptr<GameObject>& obj)
{
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/player_True/player.mdl");
    r->SetDissolveThreshold(1.0f);
    obj->AddComponent<AimIKCom>("spine2", nullptr);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/player.nodecollsion");
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<CharaStatusCom> status = obj->AddComponent<CharaStatusCom>();

    //HP�̏����ݒ�
    status->SetMaxHitPoint(200);
    status->SetHitPoint(status->GetMaxHitpoint());
    status->SetInvincibleTime(0.3f);
    std::shared_ptr<FarahCom> c = obj->AddComponent<FarahCom>();
    c->SetCharaID(int(CHARA_LIST::FARAH));

    //�{�b�N�X�R���C�_�[
    std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    if (std::strcmp(obj->GetName(), "player") == 0)
        box->SetMyTag(COLLIDER_TAG::Player);
    else
        box->SetMyTag(COLLIDER_TAG::Enemy);

    //�����o������
    auto& pushBack = obj->AddComponent<PushBackCom>();
    pushBack->SetRadius(0.5f);
    pushBack->SetWeight(1);

    //���̃G�t�F�N�g
    {
        std::shared_ptr<GameObject> smoke = obj->AddChildObject();
        smoke->SetName("smokeeffect");
        std::shared_ptr<CPUParticle> smokeeffct = smoke->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/smoke.cpuparticle", 100);
        smokeeffct->SetActive(false);
    }

    //SE
    {
        auto& au = obj->AddComponent<AudioCom>();
        au->RegisterSource(AUDIOID::PLAYER_ATTACKULTBOOM, "P_ATTACK_ULT_BOOM");
        au->RegisterSource(AUDIOID::PLAYER_ATTACKULTSHOOT, "P_ATTACKULTSHOOT");
        au->RegisterSource(AUDIOID::PLAYER_CHARGE, "P_CHARGE");
        au->RegisterSource(AUDIOID::PLAYER_DAMAGE, "P_DAMAGE");
        au->RegisterSource(AUDIOID::PLAYER_DASH, "P_DASH");
        au->RegisterSource(AUDIOID::PLAYER_SHOOT, "P_SHOOT");
    }

    //�r�ƃJ�����̏����J�������v���C���[�̎q�ǂ��ɂ��Đ��䂷��
    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        std::shared_ptr<GameObject> playerObj = GameObjectManager::Instance().Find("player");
        std::shared_ptr<GameObject> cameraPost = playerObj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();
        fpscamera->ActiveCameraChange();

        //�J�����ʒu
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
            armChild->AddComponent<AnimationCom>();
        }
    }
}

void RegisterChara::JankratChara(std::shared_ptr<GameObject>& obj)
{
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/player_True/player.mdl");
    r->SetDissolveThreshold(1.0f);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/player.nodecollsion");
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<CharaStatusCom> status = obj->AddComponent<CharaStatusCom>();
    std::shared_ptr<JankratCharacterCom> charaCom = obj->AddComponent<JankratCharacterCom>();

    std::shared_ptr<GameObject> cameraPost = obj->AddChildObject();
    cameraPost->SetName("cameraPostPlayer");
    std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();
    fpscamera->ActiveCameraChange();

    //�J�����ʒu
    cameraPost->transform_->SetWorldPosition({ 0, 12.086f, 3.3050f });
    obj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());
}