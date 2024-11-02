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
class ScenePVE :public Scene
{
public:
    ScenePVE() {};
    ~ScenePVE()override {};

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "ScenePVE"; };
private:
    Light* mainDirectionalLight = nullptr;
    std::unique_ptr<BasicsApplication> photonNet;
    std::shared_ptr<TransformCom>t;
};
