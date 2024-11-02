#pragma once

#include "../Source/GameSource/Scene/Scene.h"
#include "Graphics\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Graphics/Light/Light.h"
#include "Components/NodeCollsionCom.h"

#include "Netwark/Photon/BasicsApplication.h"

//�g���[�j���O�V�[��
class SceneTraining :public Scene
{
public:
    SceneTraining() {};
    ~SceneTraining() override {};

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneTraining"; };

private:
    Light* mainDirectionalLight = nullptr;
};
