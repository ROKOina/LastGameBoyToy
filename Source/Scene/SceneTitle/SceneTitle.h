#pragma once

#include "Scene/Scene.h"
#include "Component\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Component\System\GameObject.h"
#include "Graphics/Light/Light.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"

//�^�C�g���V�[��
class SceneTitle :public Scene
{
public:
    SceneTitle() {};
    ~SceneTitle()override {};

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneTitle"; };

private:
    void UIUpdate(float elapsedTime);

private:
    Light* mainDirectionalLight = nullptr;
};