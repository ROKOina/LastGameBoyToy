#pragma once

#include "Component\System\Component.h"

class Light :public Component
{
public:

    Light(const char* filename);
    ~Light() {};

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Light"; }

public:

    // �����^�C�v
    enum class LightType
    {
        Directional,	// ���s����
        Point,			// �_����
        Spot,			// �X�|�b�g���C�g
    };
};