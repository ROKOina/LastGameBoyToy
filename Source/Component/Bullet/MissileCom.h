#pragma once

#include "../System\Component.h"
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Math\Mathf.h"
#include "Input\Input.h"
#include "Component\System\HitProcessCom.h"

class MissileCom : public Component
{
public:
    MissileCom(){};
    ~MissileCom() override {};

    // ���O�擾
    const char* GetName() const override { return "Missile"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

private:
    bool playFlag = false;
};