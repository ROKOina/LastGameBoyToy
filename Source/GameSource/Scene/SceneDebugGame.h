#pragma once

#include <memory>
#include "Scene.h"
#include "GameSource\Sprite\SpriteObject.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Components\System\StateMachine.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
#include <array>

//�O���錾
class SceneDebugGame;

enum class SCENE_ACT
{
    LOGIN,
    CHARACTER_SLECT,
    BATTLE,
};

class SceneState_Login : public State<SceneDebugGame>
{
    void Enter() override;
    void Update() override;
};

// �Q�[���V�[��
class SceneDebugGame :public Scene
{
public:
    SceneDebugGame() {}
    ~SceneDebugGame()override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

private:
    // �e�v���C���[�̓��͏����A���ꂼ��̃L�����N�^�[�ɑ���
    void SetUserInputs();

    // �v���C���[( PC�̎����� )�̓��͏��
    void SetPlayerInput();

    // ���̃v���C���[( �I�����C�� )�̓��͏��
    void SetOnlineInput();

public:
    static constexpr int MAX_PLAYER_NUM = 6;


    std::array<std::weak_ptr<GameObject>, MAX_PLAYER_NUM> GetPlayers() { return players; }

private:
    std::unique_ptr<NetwarkPost> n;
    Light* mainDirectionalLight = nullptr;

    SpriteObject* sprite = nullptr;

    StateMachine<SceneActionState, SCENE_ACT> stateMachine;
    std::array<std::weak_ptr<GameObject>,MAX_PLAYER_NUM> players;
};


