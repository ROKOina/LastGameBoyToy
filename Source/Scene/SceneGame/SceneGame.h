#pragma once

#include "Scene\Scene.h"
#include "Component\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Component\System\GameObject.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"

// �Q�[���V�[��
class SceneGame :public Scene
{
public:
    SceneGame() {}
    ~SceneGame()override;

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //�G�t�F�N�g����
    void EffectNew();

    //���O�擾
    std::string GetName() const override { return "SceneGame"; };

private:

    //UI�̃Q�[���I�u�W�F�N�g����
    void CreateUiObject();

    //PVE�Q�[�����烊�U���g�ɑJ��
    void TransitionPVEFromResult();
private:
    //std::unique_ptr<NetwarkPost> n;
    std::unique_ptr<BasicsApplication> photonNet;

private:

    std::shared_ptr<TransformCom>t;
};