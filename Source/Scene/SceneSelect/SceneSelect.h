#pragma once

#include "Scene/Scene.h"
#include "Component\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Component\System\GameObject.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"

//�^�C�g���V�[��
class SceneSelect :public Scene
{
public:
    SceneSelect() {};
    ~SceneSelect()override {};

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneSelect"; };

private:
    void UIUpdate(float elapsedTime);
};