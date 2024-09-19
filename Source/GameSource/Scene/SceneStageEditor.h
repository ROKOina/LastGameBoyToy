#pragma once

#include "Scene.h"
#include "Graphics\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
#include "Components/NodeCollsionCom.h"

#include "Netwark/Photon/BasicsApplication.h"

class StageObj;

// �Q�[���V�[��
class SceneStageEditor : public Scene
{
public:
    SceneStageEditor() {}
    ~SceneStageEditor()override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneGame"; };

private:
    std::vector<StageObj*> stageObjcts;
    Light* mainDirectionalLight = nullptr;
};

class StageObj
{
public:
    StageObj(std::string mdl_name)
    {
        name = mdl_name;
        model = std::make_unique<Model>(mdl_name);
    }

    Model* GetModel() { return model.get(); }

private:
    std::string name;
    std::unique_ptr<Model> model;
};
