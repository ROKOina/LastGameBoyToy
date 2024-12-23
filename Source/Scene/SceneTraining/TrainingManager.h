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
    void ChangeTrainigFlag();


    //void LightManager();

    void Changeblackout();
    void Changelightchange();
private:

    //�ŏ��̓g���[�j���O���[�h
    bool tutorialFlag = false;

    bool lightFlag = false;

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
    void TrainingObjDisplay();


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
    {BLACK, LIGHT, MOVE, GUN, SKILL, ULT, ENDBLACK, END };
public:
    //�`���[�g���A���Ǘ��V�X�e��
    void TutorialManagerSystem(float elapsedTime);

    //�`���[�g���A����i�߂�
    void NextTutorial(TutorialID id);

    void TutorialFlagClear();

    void TutorialUIUnhind();
    void TutorialUIDisplay();


public:
    //�Ó]
    void BlackOutManager(float elapsedTime);

    //���]
   void LightChangeManger(float elapsedTime);

    //�����̃`���[�g���A���Ǘ�
    void MoveTutorialManager(float elapsedTime);

    //�e�̃`���[�g���A���Ǘ�
    void GunTutorialManager(float elapsedTime);

    //�X�L���̃`���[�g���A���Ǘ�
    void SkillTutorialManager(float elapsedTime);

    //�E���g�̃`���[�g���A���Ǘ�
    void UltTutorialManager(float elapsedTime);

    void EndBlackTutorialManager(float elapsedTime);

    void EndTutorialManager(float elapsedTime);

   

    void OnGui();
private:

    int tutorialID = 0;
    
    int  moveSubTitleIndex = 0;
    int  gunSubTitleIndex = 0;
    int  skillSubTitleIndex = 0;
    int  ultSubTitleIndex = 0;

    float lightTimer = 0.0f;
    float lightTime = 2.0f;
    float blackTimer = 0.0f;
    float blackTime = 2.0f;

    bool moveInspectionFlag = false;         //�����̃`���[�g���A������
    bool gunInspectionFlag = false;          //�e�̃`���[�g���A������
    bool skillInspectionFlag = false;        //�X�L���̃`���[�g���A������
    bool ultInspectionFlag = false;          //�E���g�̃`���[�g���A������

    bool flag = false;

    bool upFlag = false;
    bool downFlag = false;
    bool jumpFlag = false;
    bool gunFlag = false;
    bool skillFlag = false;
    bool ultFlag = false;

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
        

        DirectX::XMFLOAT2 pos = { 0,0 };

        std::wstring str;
    };

    SubTitle moveSubTitle[12];
    SubTitle gunSubTitle[4];
    SubTitle skillSubTitle[4];
    SubTitle ultSubTitle[5];

    

    
};