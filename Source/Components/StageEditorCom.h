#pragma once
#include "./System/Component.h"
#include "./ColliderCom.h"
#include <List>
#include <map>

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

    //�z�u����I�u�W�F�N�g��o�^
    void ObjectRegister();

    //�}�E�X�ƃX�e�[�W�̔���
    bool MouseVsStage(HitResult& hit);

    void ObjectSave();
    void ObjectLoad();

private:

    struct PlaceObject
    {
        std::string filePath;
        std::list<GameObj> objList;
    };

private:
    bool onImGui = false;//�J�[�\����Gui��ɂ��邩�ǂ���
    bool nowEdit = false;//�ҏW���t���O

    std::map<std::string, PlaceObject> placeObjcts;//�I�u�W�F�N�g�̖��O��Modle�̃t�@�C���p�X��R�Â��ĕۑ�
    char registerObjName[256] = {};
    std::string objType;
    std::list<GameObj> objList;
};