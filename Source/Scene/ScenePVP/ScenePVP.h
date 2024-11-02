#pragma once

#include "Scene/Scene.h"
#include "Component\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"
#include "Component\System\GameObject.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"

//���U���g�V�[��
class ScenePVP :public Scene
{
public:
    ScenePVP() {};
    ~ScenePVP()override {};

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "ScenePVP"; };
private:
    Light* mainDirectionalLight = nullptr;
};
