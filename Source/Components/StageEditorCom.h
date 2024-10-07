#pragma once
#include "./System/Component.h"
#include "./ColliderCom.h"
#include <List>

class StageEditorCom : public Component
{
public:

    // ���O�擾
    const char* GetName() const override { return "StageEditor"; }

    // �J�n����
    void Start() {}

    // �X�V����
    void Update(float elapsedTime);

    // �j������
    void OnDestroy() {}

    // GUI�`��
    void OnGUI();

private:
    //�G�f�B�^�[�ł�����X�e�[�W��I��
    void StageSelect();
    //�z�u����I�u�W�F�N�g��o�^
    void ObjectRegister();

    //�X�e�[�W�ɃI�u�W�F�N�g�z�u
    void ObjectPlace();
    //�}�E�X�ƃX�e�[�W�̔���
    bool MouseVsStage(HitResult& hit);

    void ObjectSave();
    void ObjectLoad();

private:
    bool onImGui = false;//�J�[�\����Gui��ɂ��邩�ǂ���
    bool nowEdit = false;//�ҏW���t���O

    std::list<GameObj> objList;
};