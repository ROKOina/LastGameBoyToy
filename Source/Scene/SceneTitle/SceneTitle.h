#pragma once

#include "Scene/Scene.h"
#include "Component\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Component\System\GameObject.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"
#include "Component\Audio\AudioCom.h"

//�^�C�g���V�[��
class SceneTitle :public Scene
{
public:
    SceneTitle() {};
    ~SceneTitle()override;

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

    //ui�̍X�V
    void UIUpdate(float elapsedTime);

    //��ʃG�t�F�N�g����
    void ScreenEffect(float elapsedTime);

    AudioCom* audioObj;
};