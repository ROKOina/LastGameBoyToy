#pragma once
#include "Component\System/Component.h"
#include <List>
#include <map>
#include <Math\Collision.h>
#include <functional>
#include "Component\Renderer\RendererCom.h"

using GenerateFunc = std::function<void(GameObj my)>;

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

    //Json�̃f�[�^�����ɃI�u�W�F�N�g�z�u
    void PlaceJsonData(std::string filename);

private:
    //�z�u����I�u�W�F�N�g��o�^
    void ObjectRegister();

    //�X�e�[�W�ɃI�u�W�F�N�g�z�u
    GameObj ObjectPlace(
        std::string objType,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT4 rotation,
        const char* model_filename,
        const char* collision_filename
    );

    //�t�@�C���p�X�擾
    void FileRead(std::string& path);
    //�}�E�X�ƃX�e�[�W�̔���
    bool MouseVsStage(HitResult& hit);

    void ObjectSave();
    void ObjectLoad();

private:
    //�Q�[���I�u�W�F�N�g�����֐��̖��O
    enum class GenerateFuncName
    {
        None = 0,//�ݒ肵�Ȃ�

        TestNakanisi,
        TowerGimic,
        Max,
    };

    //�Q�[���I�u�W�F�N�g�����֐�
    static void TestNakanisi(GameObj place);
    static void TowerGimic(GameObj& place);

    GenerateFunc generateFunc[(int)GenerateFuncName::Max] =
    {
        nullptr, //None

        TestNakanisi,
        TowerGimic
    };

    //�ۑ��p�̃f�[�^���i�[����\����
    struct PlaceObject
    {
        bool staticFlag = false;
        std::string filePath;
        std::string collisionPath;
        std::list<GameObj> objList;

        GenerateFuncName func;
    };

private:
    bool onImGui = false;//�J�[�\����Gui��ɂ��邩�ǂ���
    bool nowEdit = false;//�ҏW���t���O

    std::map<std::string, PlaceObject> placeObjcts;//�I�u�W�F�N�g�̖��O��Modle�̃t�@�C���p�X��R�Â��ĕۑ�
    char registerObjName[256] = {};
    std::string objType;
    std::list<GameObj> objList;
};
