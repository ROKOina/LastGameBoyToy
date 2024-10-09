#pragma once

#include "Components/System/Component.h"

class SpawnCom :public Component
{
public:

    SpawnCom();
    ~SpawnCom() {};

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Spawn"; }

    //�����@�\
    void SpawnGameObject();

private:

    //�����Ԋu
    float spawnInterval = 5.0f;  // �f�t�H���g�ł�5�b���Ƃɐ���
    float spawnRadius = 5.0f;    // �����_�������͈͂̔��a

    // ������
    int spawnCount = 5;         // �f�t�H���g�̐�������10

    // �Ō�ɐ�����������
    float lastSpawnTime = 0.0f;

    // ���ݐ������ꂽ�I�u�W�F�N�g�̃J�E���g
    int currentSpawnedCount = 0; // �������ꂽ�I�u�W�F�N�g�̐���ǐ�
};