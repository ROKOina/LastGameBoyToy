#pragma once

#include "Scene.h"
#include "Graphics\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
#include "Components/NodeCollsionCom.h"

#include "Components\ColliderCom.h"
#include <Windows.h>
#include <map>
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
    //imgui
    void ImGui();
    // ���f������
    GameObj ImportModel(const char* filename);

    //�G�f�B�^�[�ł�����X�e�[�W��I��
    void StageSelect();
    //�z�u����I�u�W�F�N�g��o�^
    void ObjectRegister();

    //�X�e�[�W�ɃI�u�W�F�N�g�z�u
    void ObjectPlace();
    //�}�E�X�ƃX�e�[�W�̔���
    bool MouseVsStage(HitResult hit);

private:
    GameObj editorObj;
    GameObj stageObj;
    
    char registerObjName[256] = {};
    std::string selectObjName;
    std::map<std::string, std::string> gameObjcts;

    Light* mainDirectionalLight = nullptr;
};