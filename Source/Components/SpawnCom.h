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

public:

    //�X�V���邩�ۂ�
    const bool OnTrigger() { return spwntrigger; }
    void SetOnTrigger(bool flag) { spwntrigger = flag; }

private:

    //�����Ԋu
    float spawnInterval = 0.6f;
    float spawnRadius = 5.0f;

    // ������
    int spawnCount = 2;

    // �Ō�ɐ�����������
    float lastSpawnTime = 0.0f;

    // ���ݐ������ꂽ�I�u�W�F�N�g�̃J�E���g
    int currentSpawnedCount = 0; // �������ꂽ�I�u�W�F�N�g�̐���ǐ�

    //�����t���O
    bool spwntrigger = false;
};