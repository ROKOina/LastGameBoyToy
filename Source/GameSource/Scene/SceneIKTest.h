#pragma once

#include "Scene.h"
#include "Graphics\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"

// �Q�[���V�[��
class SceneIKTest :public Scene
{
public:
    SceneIKTest() {}
    ~SceneIKTest()override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override {};

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneIKTest"; };

private:
    Light* mainDirectionalLight = nullptr;
};
