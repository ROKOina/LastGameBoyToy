#include "SpawnCom.h"
#include "Components/TransformCom.h"
#include "Components/RendererCom.h"
#include "Components/AnimationCom.h"
#include <cstdlib>
#include <cmath>
#include "Components/Enemy/NoobEnemy/NoobEnemyCom.h"
#include "Components/AimIKCom.h"
#include "Components/NodeCollsionCom.h"

// ������
SpawnCom::SpawnCom() : currentSpawnedCount(0)
{
}

// �����ݒ�
void SpawnCom::Start()
{
}

// �X�V����
void SpawnCom::Update(float elapsedTime)
{
    // ���ԍX�V
    lastSpawnTime += elapsedTime;

    // �����Ԋu�𒴂����ꍇ�ɃI�u�W�F�N�g�𐶐�
    if (lastSpawnTime >= spawnInterval)
    {
        //�����I�u�W�F�N�g�𐶐�
        for (int i = 0; i < spawnCount; ++i)
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
    ImGui::Text("Spawn Settings");
    ImGui::SliderFloat("Spawn Interval", &spawnInterval, 1.0f, 10.0f);
    ImGui::SliderFloat("Spawn Radius", &spawnRadius, 1.0f, 10.0f);
    ImGui::SliderInt("Spawn Count", &spawnCount, 1, 100);
}

//�Q�[���I�u�W�F�N�g�𕡐����鏈��
void SpawnCom::SpawnGameObject()
{
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    obj->SetName("NoobEnemy");

    //�X�P�[���ݒ�
    obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });

    // �I�u�W�F�N�g�ɔԍ���t����
    std::string objectName = std::string(obj->GetName()) + "_" + std::to_string(currentSpawnedCount);
    obj->SetName(objectName.c_str());

    //�e�I�u�W�F�N�g�̈ʒu
    DirectX::XMFLOAT3 originalPosition = GetGameObject()->transform_->GetWorldPosition();

    // ���aspawnRadius���[�g���ȓ��̃����_���Ȉʒu���v�Z
    float randomAngle = static_cast<float>(rand()) / RAND_MAX * 2.0f * DirectX::XM_PI; // 0����2�΂܂ł̃����_���p�x
    float randomDistance = static_cast<float>(rand()) / RAND_MAX * spawnRadius;        // 0����spawnRadius�܂ł̃����_������

    // 2D���ʏ�Ń����_���ʒu���v�Z
    float offsetX = cosf(randomAngle) * randomDistance;
    float offsetZ = sinf(randomAngle) * randomDistance;

    // �V�����ʒu��ݒ�i�����͌��̈ʒu��Y���W���ێ��j
    DirectX::XMFLOAT3 newPosition = {
        originalPosition.x + offsetX,
        originalPosition.y + 0.2f,  // �������ێ�
        originalPosition.z + offsetZ
    };
    obj->transform_->SetWorldPosition(newPosition);

    // ���̃R���|�[�l���g�����l�ɒǉ�
    std::shared_ptr<RendererCom> renderer = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    renderer->LoadModel("Data/Jammo/jammo.mdl");
    obj->AddComponent<MovementCom>();
    obj->AddComponent<NodeCollsionCom>("Data/Jammo/jammocollsion.nodecollsion");
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<AimIKCom>(nullptr, "mixamorig:Neck");
    obj->AddComponent<NoobEnemyCom>();

    // ���݂̐��������C���N�������g
    currentSpawnedCount++;
}