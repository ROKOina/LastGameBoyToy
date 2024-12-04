#pragma once

#include "Component/System/Component.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\Collsion\ColliderCom.h"

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
    void SpawnGameObject(float elapsedTime);

private:

    //�V���A���C�Y
    void Serialize();

    // �f�V���A���C�Y
    void Deserialize(const char* filename);

    // �f�V���A���C�Y�̓ǂݍ���
    void LoadDeserialize();

    //�����_���ʒu
    DirectX::XMFLOAT3 SpawnCom::GenerateRandomPosition();

    //�ʂŖ��O��t����
    void AssignUniqueName(const std::shared_ptr<GameObject>& obj);

    //�G�l�~�[�����֐�
    void SetupEnemy(const std::shared_ptr<GameObject>& obj);

    //�~�T�C�������֐�
    void SetupMissile(const std::shared_ptr<GameObject>& obj);

    //���������֐�
    void SetupExplosion(const std::shared_ptr<GameObject>& obj);

    //�r�[�������֐�
    void SetupBeam(const std::shared_ptr<GameObject>& obj);
    void CreateBeamSegment(const std::shared_ptr<GameObject>& origin, const char* easingMovePath);

    //�M�~�b�N�~�T�C�������֐�
    void CreateGimmickMissile(const std::shared_ptr<GameObject>& obj);

    //�t�@���̃E���g
    void CreateFarahUlt(const std::shared_ptr<GameObject>& obj);

    //�����蔻��
    void HitObject();

    //�n�ʂɒ��n���������
    void OnGroundDelete(float elapsedTime);

public:

    //�X�V���邩�ۂ�
    bool OnTrigger() const { return spwntrigger; }
    void SetOnTrigger(bool flag) { spwntrigger = flag; }

    //�������ꂽ�u�Ԃ̃t���O
    bool GetSpawnFlag()const { return spawnflag; }
    void SetSpawnFlag(bool flag) { spawnflag = flag; }

public:

    //�I�u�W�F�N�g��enum
    enum class ObjectType
    {
        ENEMY,
        MISSILE,
        EXPLOSION,
        BEEM,
        GIMMICKMISSILE,
        FARAHULT,
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
    bool spawnflag = false;
    std::string filepath = {};

    // �������ꂽ�I�u�W�F�N�g���Ǘ����郊�X�g
    std::vector<std::weak_ptr<GameObject>> spawnedObjects;
    std::vector<std::pair<std::weak_ptr<GameObject>, float>> deleteQueue; // �폜�\��L���[
};