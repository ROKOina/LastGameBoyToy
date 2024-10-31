#include "SpawnCom.h"
#include "Components/TransformCom.h"
#include "Components/AnimationCom.h"
#include <cstdlib>
#include <cmath>
#include <fstream>
#include "Components/Enemy/NoobEnemy/NoobEnemyCom.h"
#include "Components/FrustumCom.h"
#include "Components/AimIKCom.h"
#include "Components/NodeCollsionCom.h"
#include "Components\Character\CharaStatusCom.h"
#include "Components\EasingMoveCom.h"
#include "Dialog.h"
#include "Logger.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include "Components/PushBackCom.h"
#include <random>

CEREAL_CLASS_VERSION(SpawnCom::SpawnParameter, 1)

// �V���A���C�Y
namespace DirectX
{
    template<class Archive>
    void serialize(Archive& archive, XMUINT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

template<class Archive>
void SpawnCom::SpawnParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(spawnInterval),
        CEREAL_NVP(spawnRadius),
        CEREAL_NVP(spawnCount),
        CEREAL_NVP(objecttype),
        CEREAL_NVP(Yoffset),
        CEREAL_NVP(distanceXY)
    );
}

// ������
SpawnCom::SpawnCom(const char* filename) : currentSpawnedCount(0)
{
    //�ǂݍ���
    if (filename)
    {
        Deserialize(filename);
    }
}

// �X�V����
void SpawnCom::Update(float elapsedTime)
{
    if (spwntrigger)
    {
        // ���ԍX�V
        lastSpawnTime += elapsedTime;
    }
    else
    {
        lastSpawnTime = 0.0f;
    }

    // �����Ԋu�𒴂����ꍇ�ɃI�u�W�F�N�g�𐶐�
    if (lastSpawnTime >= sp.spawnInterval)
    {
        //�����I�u�W�F�N�g�𐶐�
        for (int i = 0; i < sp.spawnCount; ++i)
        {
            SpawnGameObject();
        }

        // �^�C�}�[�����Z�b�g
        lastSpawnTime = 0.0f;
    }
}

// imgui
void SpawnCom::OnGUI()
{
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"�ۑ�"))
    {
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"�ǂݍ���"))
    {
        LoadDeserialize();
    }
    ImGui::SameLine();
    ImGui::Checkbox((char*)u8"�����t���O", &spwntrigger);

    if (ImGui::TreeNode((char*)u8"�������̃p�����[�^"))
    {
        constexpr const char* objectTypeItems[] = { "ENEMY", "MISSILE" };
        static_assert(ARRAYSIZE(objectTypeItems) == static_cast<int>(ObjectType::MAX), "objectTypeItems Size Error!");
        ImGui::Combo((char*)u8"�I�u�W�F�N�g�^�C�v", &sp.objecttype, objectTypeItems, static_cast<int>(ObjectType::MAX));
        objtype = static_cast<ObjectType>(sp.objecttype);
        sp.objecttype = static_cast<int>(objtype);
        ImGui::DragFloat((char*)u8"���v����", &lastSpawnTime);
        ImGui::DragFloat((char*)u8"�����Ԋu", &sp.spawnInterval, 0.1f, 0.0f, 10.0f);
        ImGui::DragFloat((char*)u8"�������a", &sp.spawnRadius, 0.0f, 0.0f, 50.0f);
        ImGui::DragInt((char*)u8"������", &sp.spawnCount, 1, 0, 100);
        ImGui::DragFloat((char*)u8"Y���I�t�Z�b�g", &sp.Yoffset, 0.0f, 0.0f, 20.0f);
        ImGui::DragFloat2((char*)u8"�ŏ��ő�", &sp.distanceXY.x, 0.1f, 0.0f, 20.0f);
        ImGui::TreePop();
    }
}

//�Q�[���I�u�W�F�N�g�𕡐����鏈��
void SpawnCom::SpawnGameObject()
{
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    std::shared_ptr<SphereColliderCom> collider;
    std::shared_ptr<RendererCom> renderer;
    std::shared_ptr<CPUParticle>cpuparticle;
    std::shared_ptr<PushBackCom>pushback;

    // �e�I�u�W�F�N�g�̈ʒu���擾
    DirectX::XMFLOAT3 originalPosition = GetGameObject()->transform_->GetWorldPosition();

    //�ǂ̃I�u�W�F�N�g�𐶐����邩���肷��
    objtype = static_cast<ObjectType>(sp.objecttype);
    sp.objecttype = static_cast<int>(objtype);
    switch (objtype)
    {
    case ObjectType::ENEMY:

        obj->SetName("NoobEnemy");
        obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
        renderer = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        renderer->LoadModel("Data/Jammo/jammo.mdl");
        obj->AddComponent<MovementCom>();
        obj->AddComponent<NodeCollsionCom>("Data/Jammo/jammocollsion.nodecollsion");
        obj->AddComponent<AnimationCom>();
        obj->AddComponent<AimIKCom>(nullptr, "mixamorig:Neck");
        obj->AddComponent<NoobEnemyCom>();
        obj->AddComponent<CharaStatusCom>();
        collider = obj->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        pushback = obj->AddComponent<PushBackCom>();
        pushback->SetRadius(0.5f);
        pushback->SetWeight(0.5f);

        break;

    case ObjectType::MISSILE:

        obj->SetName("fireball");
        cpuparticle = obj->AddComponent<CPUParticle>("Data/Effect/fireball.cpuparticle", 1000);
        cpuparticle->SetActive(true);
        obj->AddComponent<EasingMoveCom>("Data/3DEasingData/missile.easingmove");

        break;

    default:

        collider = nullptr;
        renderer = nullptr;
        cpuparticle = nullptr;

        break;
    }

    // �I�u�W�F�N�g�ɔԍ���t����
    std::string objectName = std::string(obj->GetName()) + "_" + std::to_string(currentSpawnedCount);
    obj->SetName(objectName.c_str());

    // �����_���Ȉʒu�𐶐�
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * DirectX::XM_PI);
    std::uniform_real_distribution<float> distanceDist(sp.distanceXY.x, sp.distanceXY.y);

    float randomAngle = angleDist(gen);
    float randomDistance = std::sqrt(distanceDist(gen)) * sp.spawnRadius; // �������ŋ������z�𒲐�

    float offsetX = std::cos(randomAngle) * randomDistance;
    float offsetZ = std::sin(randomAngle) * randomDistance;

    // �V�����ʒu�����̈ʒu�ɃI�t�Z�b�g�������Đݒ�
    DirectX::XMFLOAT3 newPosition =
    {
        originalPosition.x + offsetX,
        originalPosition.y + sp.Yoffset,
        originalPosition.z + offsetZ
    };
    obj->transform_->SetWorldPosition(newPosition);

    // ���݂̐��������C���N�������g
    currentSpawnedCount++;
}

//�V���A���C�Y
void SpawnCom::Serialize()
{
    static const char* filter = "Spawn Files(*.spawn)\0*.spawn;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "spawn", Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        std::ofstream ostream(filename, std::ios::binary);
        if (ostream.is_open())
        {
            cereal::BinaryOutputArchive archive(ostream);

            try
            {
                archive
                (
                    CEREAL_NVP(sp)
                );
            }
            catch (...)
            {
                LOG("spawn deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

// �f�V���A���C�Y
void SpawnCom::Deserialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(sp)
            );
        }
        catch (...)
        {
            LOG("spawn deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

// �f�V���A���C�Y�̓ǂݍ���
void SpawnCom::LoadDeserialize()
{
    static const char* filter = "Spawn Files(*.spawn)\0*.spawn;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Deserialize(filename);
    }
}