#pragma once
#include "../System\Component.h"
#include "../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"

class CharacterCom : public Component
{
public:
    enum class CHARACTER_ACTIONS {
        IDLE,
        MOVE,
        JUMP,
        MAX,
    };

public:
    CharacterCom() {};
    ~CharacterCom() override {};

    // ���O�擾
    const char* GetName() const override { return "Character"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

    StateMachine<CharacterCom, CHARACTER_ACTIONS>& GetStateMachine() { return stateMachine; }

protected:
    StateMachine<CharacterCom, CHARACTER_ACTIONS> stateMachine;

    bool jumpFlag = false;
};
