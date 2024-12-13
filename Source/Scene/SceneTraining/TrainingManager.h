#pragma once
#include "Component/System/Component.h"




//�g���[�j���O�̓���
class TrainingManager
{
private:
    TrainingManager();
    ~TrainingManager();
public:
    //�B��̃C���X�^���X�擾
    static TrainingManager& Instance()
    {
        static TrainingManager instance;
        return instance;
    }

    void TrainingManagerUpdate(float elapsedTime);
    void TrainingManagerStart();
    void TrainingManagerClear();
    void OnGUI();

public:
    void ChangeTutorialFlag();
private:

    //�ŏ��̓g���[�j���O���[�h
    bool tutorialFlag = false;

};

//�g���[�j���O���[�h
class TrainingSystem
{
private:
    TrainingSystem();
    ~TrainingSystem();
public:
    //�B��̃C���X�^���X�擾
    static TrainingSystem& Instance()
    {
        static TrainingSystem instance;
        return instance;
    }

    void TrainingSystemUpdate(float elapsdTime);
    void TrainingSystemStart();
    void TrainingSystemClear();

public:

    void SetItemFlag(bool flag) { itemFlag = flag; }

    //�g���[�j���O���[�h�̃I�u�W�F�N�g���\��
    void TrainingObjUnhide();

private:
    //�ˌ��̊J�n�����ƏI������
    void ShootingStartEndSystem();

    //�ˌ��̃��W�b�N�H
    void ShootingSystem(float elapsdTime);

    //�ˌ����̈ĎR�q�X�|�[�� 
    void ShootingSpawnCrow();

    //�A�C�e���̃X�|�[��
    void SpawnItem();

    //�A�C�e���X�|�[���V�X�e��
    void SpawnItemSystem(float elapsdTime);

    
private:
    int     shootingScore = 0;                     //�ˌ��̃X�R�A
    int     scarecrowMaxTotal = 30;                //�ˌ����̍ő�ĎR�q�X�|�[����
    int     scarecrowCount = 0;                    //�ˌ����̈ĎR�q�X�|�[��
          
    float   scarecrowLifeTime = 1.0f;              //�X�|�[�����Ă���̐�������
    float   scarecrowLifeTimer = 0.0f;             //�������ԃ^�C�}�[
    float   scarecrowSpawnIntervalTime  = 1.0f;    //�ĎR�q���|����Ă���̎��̈ĎR�q���X�|�[���̊Ԋu
    float   scarecrowSpawnIntervalTimer = 0.0f;    //���̃^�C�}�[
    float   spawnItemIntervalTime = 2.0f;          //�A�C�e�����擾����Ă���̎��̃A�C�e�����X�|�[��������Ԋu
    float   spawnItemIntervalTimer = 0.0f;         //���̃^�C�}�[

    bool    shootingStartFlag = false;             //�ˌ����J�n���I�����@false=�I���@True���J�n
    bool    shootingIntervalFlag = false;          //���̈ĎR�q���X�|�[��������M��
    bool    itemFlag = true;                      //�A�C�e�������邩�ǂ���
};

//�`���[�g���A�����[�h
class TutorialSystem
{
private:
    TutorialSystem();
    ~TutorialSystem();
public:
    //�B��̃C���X�^���X�擾
    static TutorialSystem& Instance()
    {
        static TutorialSystem instance;
        return instance;
    }

    void TutorialSystemUpdate(float elapsedTime);
    void TutorialSystemStart();
    void TutorialSystemClear();

public:
    //�`���[�g���A����ID
    enum TutorialID
    { MOVE, GUN, SKILL, ULT, END };
public:
    //�`���[�g���A���Ǘ��V�X�e��
    void TutorialManagerSystem(float elapsedTime);

    //�`���[�g���A����i�߂�
    void NextTutorial(TutorialID id);
public:
    //�����̃`���[�g���A���Ǘ�
    void MoveTutorialManager(float elapsedTime);

    //�e�̃`���[�g���A���Ǘ�
    void GunTutorialManager(float elapsedTime);

    //�X�L���̃`���[�g���A���Ǘ�
    void SkillTutorialManager(float elapsedTime);

    //�E���g�̃`���[�g���A���Ǘ�
    void UltTutorialManager(float elapsedTime);

    void OnGui();
private:

    int tutorialID = 0;
    
    int  moveSubTitleIndex = 0;
    int  gunSubTitleIndex = 0;
    int  skillSubTitleIndex = 0;
    int  ultSubTitleIndex = 0;

    bool moveInspectionFlag = false;         //�����̃`���[�g���A������
    bool gunInspectionFlag = false;          //�e�̃`���[�g���A������
    bool skillInspectionFlag = false;        //�X�L���̃`���[�g���A������
    bool ultInspectionFlag = false;          //�E���g�̃`���[�g���A������

    bool moveAFlag = false;
    bool moveDFlag = false;
    bool moveWFlag = false;
    bool moveSFlag = false;
    bool moveJumpFlag = false;


    DirectX::XMFLOAT2 InputVec = { 0,0 };

    struct SubTitle
    {
        int  UIID = 0;              //UI�̏o������������Ȃ������U����ő�p

        float subtitleTimer = 0.0f;
        float subtitleTime = 1.0f;
        float subtitleAlpha = 1.0f;
    };

    SubTitle moveSubTitle[11];
    SubTitle gunSubTitle[3];
    SubTitle skillSubTitle[3];
    SubTitle ultSubTitle[5];

};