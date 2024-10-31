#pragma once

#include "Components/System/Component.h"
#include "Components/RendererCom.h"
#include "Components\ColliderCom.h"
#include "Components\CPUParticle.h"

class SpawnCom :public Component
{
public:

    SpawnCom(const char* filename);
    ~SpawnCom() {};

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Spawn"; }

    //�����@�\
    void SpawnGameObject();

private:

    //�V���A���C�Y
    void Serialize();

    // �f�V���A���C�Y
    void Deserialize(const char* filename);

    // �f�V���A���C�Y�̓ǂݍ���
    void LoadDeserialize();

public:

    //�X�V���邩�ۂ�
    bool OnTrigger() const { return spwntrigger; }
    void SetOnTrigger(bool flag) { spwntrigger = flag; }

public:

    //�I�u�W�F�N�g��enum
    enum class ObjectType
    {
        ENEMY,
        MISSILE,
        MAX
    };
    ObjectType objtype = ObjectType::ENEMY;

    //�������̃p�����[�^
    struct SpawnParameter
    {
        float spawnInterval = 0.0f;//�����Ԋu
        float spawnRadius = 0.0f;  //�������a
        int spawnCount = 0;        //������
        int objecttype = 0;        //�I�u�W�F�N�g�̃^�C�v
        float Yoffset = 0.0f;      //Y���̃I�t�Z�b�g
        DirectX::XMFLOAT2 distanceXY = { 0.0f ,0.0f }; //����

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SpawnParameter sp;

private:

    // �Ō�ɐ�����������
    float lastSpawnTime = 0.0f;

    // ���ݐ������ꂽ�I�u�W�F�N�g�̃J�E���g
    int currentSpawnedCount = 0; // �������ꂽ�I�u�W�F�N�g�̐���ǐ�

    //�����t���O
    bool spwntrigger = false;
};