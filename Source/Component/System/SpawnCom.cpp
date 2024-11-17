#include "SpawnCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include <cstdlib>
#include <cmath>
#include <fstream>
#include "Component/Enemy/NoobEnemyCom.h"
#include "Component/Collsion/FrustumCom.h"
#include "Component/Animation/AimIKCom.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\MoveSystem\EasingMoveCom.h"
#include "SystemStruct\Dialog.h"
#include "SystemStruct\Logger.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include "Component/Collsion/PushBackCom.h"
#include "Component\Particle\GPUParticle.h"
#include <random>
#include "Component\Particle\CPUParticle.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\PostEffect\PostEffect.h"

CEREAL_CLASS_VERSION(SpawnCom::SpawnParameter, 1)

// シリアライズ
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

// 初期化
SpawnCom::SpawnCom(const char* filename) : currentSpawnedCount(0)
{
    //読み込み
    if (filename)
    {
        Deserialize(filename);
        filepath = filename;
    }
}

// 更新処理
void SpawnCom::Update(float elapsedTime)
{
    if (spwntrigger)
    {
        // 時間更新
        lastSpawnTime += elapsedTime;
    }
    else
    {
        lastSpawnTime = 0.0f;
    }

    // 生成間隔を超えた場合にオブジェクトを生成
    if (lastSpawnTime >= sp.spawnInterval)
    {
        //複数オブジェクトを生成
        for (int i = 0; i < sp.spawnCount; ++i)
        {
            SpawnGameObject();
            spawnflag = true;  // オブジェクト生成が行われたタイミングでフラグを true に
        }

        // タイマーをリセット
        lastSpawnTime = 0.0f;
    }
    else
    {
        spawnflag = false;
    }

    //当たり判定
    HitObject();
}

// imgui
void SpawnCom::OnGUI()
{
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"保存"))
    {
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"読み込み"))
    {
        LoadDeserialize();
    }

    // ファイルパスを表示
    char filename[256];
    ::strncpy_s(filename, sizeof(filename), filepath.c_str(), sizeof(filename));
    if (ImGui::InputText((char*)u8"ファイルパス", filename, sizeof(filename), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        filepath = filename;
    }

    ImGui::Checkbox((char*)u8"生成フラグ", &spwntrigger);
    ImGui::SameLine();
    ImGui::Checkbox((char*)u8"生成時フラグ", &spawnflag);

    if (ImGui::TreeNode((char*)u8"生成時のパラメータ"))
    {
        constexpr const char* objectTypeItems[] = { "ENEMY", "MISSILE","EXPLOSION","BEEM","GIMMICKMISSILE" };
        static_assert(ARRAYSIZE(objectTypeItems) == static_cast<int>(ObjectType::MAX), "objectTypeItems Size Error!");
        ImGui::Combo((char*)u8"オブジェクトタイプ", &sp.objecttype, objectTypeItems, static_cast<int>(ObjectType::MAX));
        objtype = static_cast<ObjectType>(sp.objecttype);
        sp.objecttype = static_cast<int>(objtype);
        ImGui::DragFloat((char*)u8"合計時間", &lastSpawnTime);
        ImGui::DragFloat((char*)u8"生成間隔", &sp.spawnInterval, 0.1f, 0.0f, 10.0f);
        ImGui::DragFloat((char*)u8"生成半径", &sp.spawnRadius, 0.0f, 0.0f, 50.0f);
        ImGui::DragInt((char*)u8"生成数", &sp.spawnCount, 1, 0, 100);
        ImGui::DragFloat((char*)u8"Y軸オフセット", &sp.Yoffset, 0.0f, 0.0f, 20.0f);
        ImGui::DragFloat2((char*)u8"最小最大", &sp.distanceXY.x, 0.1f, 0.0f, 20.0f);
        ImGui::TreePop();
    }
}

//ゲームオブジェクトを複製する処理
void SpawnCom::SpawnGameObject()
{
    // 新しいオブジェクトを作成
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    spawnedObjects.push_back(obj);  // オブジェクトをリストに追加

    // 位置設定
    DirectX::XMFLOAT3 newPosition = GenerateRandomPosition();
    obj->transform_->SetWorldPosition(newPosition);

    // オブジェクトタイプに基づいて設定
    objtype = static_cast<ObjectType>(sp.objecttype);
    sp.objecttype = static_cast<int>(objtype);

    // 各オブジェクトタイプに応じて処理を分岐
    switch (objtype)
    {
    case ObjectType::ENEMY:
        SetupEnemy(obj);
        break;
    case ObjectType::MISSILE:
        SetupMissile(obj);
        break;
    case ObjectType::EXPLOSION:
        SetupExplosion(obj);
        break;
    case ObjectType::BEEM:
        SetupBeam(obj);
        break;
    case ObjectType::GIMMICKMISSILE:
        CreateGimmickMissile(obj);
        break;
    default:
        // サポートされていないタイプの場合、処理を終了
        break;
    }

    // オブジェクトに番号を付ける
    AssignUniqueName(obj);
    currentSpawnedCount++;
}

//エネミー生成関数
void SpawnCom::SetupEnemy(const std::shared_ptr<GameObject>& obj)
{
    obj->SetName("NoobEnemy");
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    const auto& renderer = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    renderer->LoadModel("Data/Model/Enemy/Enemy.mdl");
    std::shared_ptr<MovementCom>m = obj->AddComponent<MovementCom>();
    m->SetIsRaycast(false);
    m->SetGravity(0.0f);
    m->SetFallSpeed(-0.4f);
    obj->AddComponent<NodeCollsionCom>(nullptr);
    obj->AddComponent<AnimationCom>();
    std::shared_ptr<AimIKCom>aik = obj->AddComponent<AimIKCom>(nullptr, "head");
    aik->SetOffsetY(4.3f);
    obj->AddComponent<NoobEnemyCom>();
    obj->AddComponent<CharaStatusCom>();
    std::shared_ptr<GPUParticle>gp = obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/enemyaura.gpuparticle", 1000);
    gp->Play();
    const auto& collider = obj->AddComponent<SphereColliderCom>();
    collider->SetMyTag(COLLIDER_TAG::Enemy);
    const auto& pushback = obj->AddComponent<PushBackCom>();
    pushback->SetRadius(0.5f);
    pushback->SetWeight(600.0f);
}

//ミサイル生成関数
void SpawnCom::SetupMissile(const std::shared_ptr<GameObject>& obj)
{
    obj->SetName("fireball");
    const auto& cpuparticle = obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/fireball.cpuparticle", 600);
    cpuparticle->SetActive(true);
    obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/fireball.gpuparticle", 4000);
    obj->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/fireball.nodecollsion");
    obj->AddComponent<EasingMoveCom>("Data/SerializeData/3DEasingData/missile.easingmove");

    const auto& collider = obj->AddComponent<SphereColliderCom>();
    collider->SetEnabled(true);
    collider->SetMyTag(COLLIDER_TAG::EnemyBullet);
    collider->SetJudgeTag(COLLIDER_TAG::Player);
    collider->SetRadius(0.8f);
}

//爆発生成関数
void SpawnCom::SetupExplosion(const std::shared_ptr<GameObject>& obj)
{
    obj->SetName("explosion");
    obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/explosionfire.cpuparticle", 500);
    std::shared_ptr<GameObject> chiledobj = obj->AddChildObject();
    chiledobj->SetName("explosionchildren");
    chiledobj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/explosionsmoke.cpuparticle", 500);
    const auto& gpuparticle = obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/explosion.gpuparticle", 3000);
    gpuparticle->Play();
}

//ビーム生成関数
void SpawnCom::SetupBeam(const std::shared_ptr<GameObject>& obj)
{
    obj->SetName("beam");

    // 4方向のビーム生成
    CreateBeamSegment(obj, "Data/SerializeData/3DEasingData/missileleft.easingmove");
    CreateBeamSegment(obj, "Data/SerializeData/3DEasingData/missileleft45.easingmove");
    CreateBeamSegment(obj, "Data/SerializeData/3DEasingData/missileright.easingmove");
    CreateBeamSegment(obj, "Data/SerializeData/3DEasingData/missileright45.easingmove");

    const auto& cpuparticle = obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/fireball.cpuparticle", 600);
    cpuparticle->SetActive(true);
    obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/fireball.gpuparticle", 4000);
    obj->AddComponent<EasingMoveCom>("Data/SerializeData/3DEasingData/missilestreat.easingmove");
    obj->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/fireball.nodecollsion");

    const auto& collider = obj->AddComponent<SphereColliderCom>();
    collider->SetEnabled(true);
    collider->SetMyTag(COLLIDER_TAG::EnemyBullet);
    collider->SetJudgeTag(COLLIDER_TAG::Player);
    collider->SetRadius(0.8f);
}
void SpawnCom::CreateBeamSegment(const std::shared_ptr<GameObject>& origin, const char* easingMovePath)
{
    const auto& beamSegment = GameObjectManager::Instance().Create();
    beamSegment->transform_->SetWorldPosition(origin->transform_->GetWorldPosition());

    std::string objectName = std::string(origin->GetName()) + "_" + std::to_string(currentSpawnedCount);
    beamSegment->SetName(objectName.c_str());
    beamSegment->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/fireball.nodecollsion");

    const auto& collider = beamSegment->AddComponent<SphereColliderCom>();
    collider->SetEnabled(true);
    collider->SetMyTag(COLLIDER_TAG::EnemyBullet);
    collider->SetJudgeTag(COLLIDER_TAG::Player);
    collider->SetRadius(0.8f);

    const auto& cpuparticle = beamSegment->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/fireball.cpuparticle", 600);
    cpuparticle->SetActive(true);
    beamSegment->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/fireball.gpuparticle", 4000);
    beamSegment->AddComponent<EasingMoveCom>(easingMovePath);
}

//ギミックミサイル生成関数
void SpawnCom::CreateGimmickMissile(const std::shared_ptr<GameObject>& obj)
{
    obj->SetName("gimmickmissile");
    obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/gimmickfire.cpuparticle", 1000);
    std::shared_ptr<MovementCom>move = obj->AddComponent<MovementCom>();
    move->ApplyRandomForce(55.0f, 19.0f);
    move->SetFallSpeed(-45.0f);
    move->SetFriction(2.0f);
    move->SetMoveAcceleration(30.0f);
    move->SetMoveMaxSpeed(15.0f);
}

//当たり判定
void SpawnCom::HitObject()
{
    const auto& posteffect = GameObjectManager::Instance().Find("posteffect");

    // 全ての複製されたオブジェクトに対して当たり判定を確認
    for (const auto& weakObj : spawnedObjects)
    {
        if (const auto& obj = weakObj.lock())
        {
            const auto& collision = obj->GetComponent<SphereColliderCom>();
            std::shared_ptr<Collider> collider = obj->GetComponent<Collider>();

            if (collider)
            {
                for (const auto& hitobject : collision->OnHitGameObject())
                {
                    if (const auto& hitObj = hitobject.gameObject.lock())
                    {
                        if (const auto& status = hitObj->GetComponent<CharaStatusCom>())
                        {
                            posteffect->GetComponent<PostEffect>()->SetParameter(0.9f, 70.0f, PostEffect::PostEffectParameter::VignetteIntensity);
                            GetGameObject()->transform_->SetScale({ 1.0f,1.0f,1.0f });
                            status->AddDamagePoint(-1);
                        }
                    }
                }
            }
        }
        //posteffect->GetComponent<PostEffect>()->SetParameter(0.01f, 4.0f, PostEffect::PostEffectParameter::VignetteIntensity);
    }
}

//ランダム位置
DirectX::XMFLOAT3 SpawnCom::GenerateRandomPosition()
{
    DirectX::XMFLOAT3 originalPosition = GetGameObject()->transform_->GetWorldPosition();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * DirectX::XM_PI);
    std::uniform_real_distribution<float> distanceDist(sp.distanceXY.x, sp.distanceXY.y);

    float randomAngle = angleDist(gen);
    float randomDistance = std::sqrt(distanceDist(gen)) * sp.spawnRadius;

    float offsetX = std::cos(randomAngle) * randomDistance;
    float offsetZ = std::sin(randomAngle) * randomDistance;

    return
    {
        originalPosition.x + offsetX,
        originalPosition.y + sp.Yoffset,
        originalPosition.z + offsetZ
    };
}

//個別で名前を付ける
void SpawnCom::AssignUniqueName(const std::shared_ptr<GameObject>& obj)
{
    std::string objectName = std::string(obj->GetName()) + "_" + std::to_string(currentSpawnedCount);
    obj->SetName(objectName.c_str());
}

//シリアライズ
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

// デシリアライズ
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

// デシリアライズの読み込み
void SpawnCom::LoadDeserialize()
{
    static const char* filter = "Spawn Files(*.spawn)\0*.spawn;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Deserialize(filename);
        filepath = filename;
    }
}