#include "SpawnCom.h"
#include "Components/TransformCom.h"
#include "Components/RendererCom.h"
#include "Components/AnimationCom.h"
#include <cstdlib>
#include <cmath>

// ������
SpawnCom::SpawnCom(const char* filename) : currentSpawnedCount(0)
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
    if (!prototype)
    {
        // �v���g�^�C�v���ݒ肳��Ă��Ȃ��ꍇ�͉������Ȃ�
        return;
    }

    // GameObjectManager���g���ĐV�����Q�[���I�u�W�F�N�g���쐬
    std::shared_ptr<GameObject> newObj = GameObjectManager::Instance().Create();

    // �������̏����R�s�[
    newObj->transform_->SetScale(prototype->transform_->GetScale());

    // �I�u�W�F�N�g�ɔԍ���t����
    std::string objectName = std::string(prototype->GetName()) + "_" + std::to_string(currentSpawnedCount);
    newObj->SetName(objectName.c_str());

    // ���I�u�W�F�N�g�̈ʒu
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
        originalPosition.y,  // �������ێ�
        originalPosition.z + offsetZ
    };
    newObj->transform_->SetWorldPosition(newPosition);

    // RendererCom�⑼�̃R���|�[�l���g�𕡐�
    std::shared_ptr<RendererCom> renderer = newObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    renderer->LoadModel("Data/OneCoin/robot.mdl");

    // ���̃R���|�[�l���g�����l�ɒǉ�
    std::shared_ptr<AnimationCom> anim = newObj->AddComponent<AnimationCom>();
    anim->PlayAnimation(0, true, false, 0.001f);

    // ���݂̐��������C���N�������g
    currentSpawnedCount++;
}