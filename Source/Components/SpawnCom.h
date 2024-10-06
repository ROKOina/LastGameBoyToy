#pragma once

#include "Components/System/Component.h"

class SpawnCom :public Component
{
public:

    SpawnCom(const char* filename);
    ~SpawnCom() {};

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Spawn"; }

    //��������Q�[���I�u�W�F�N�g���Z�b�g
    void SetPrototype(const std::shared_ptr<GameObject>& prototype_) { prototype = prototype_; }

    //�����@�\
    void SpawnGameObject();

private:

    //�������ƂȂ�Q�[���I�u�W�F�N�g�̃v���g�^�C�v
    std::shared_ptr<GameObject> prototype;

    //�����Ԋu
    float spawnInterval = 5.0f;  // �f�t�H���g�ł�5�b���Ƃɐ���
    float spawnRadius = 5.0f;    // �����_�������͈͂̔��a

    // ������
    int spawnCount = 10;         // �f�t�H���g�̐�������10

    // �Ō�ɐ�����������
    float lastSpawnTime = 0.0f;

    // ���ݐ������ꂽ�I�u�W�F�N�g�̃J�E���g
    int currentSpawnedCount = 0; // �������ꂽ�I�u�W�F�N�g�̐���ǐ�
};