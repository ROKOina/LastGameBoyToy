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

public:
    void SetTutorialFlag(bool flag) { tutorialFlag = flag; }
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

private:
    //�ˌ��̊J�n�����ƏI������
    void ShootingStartEndSystem();

    //�ˌ��̃��W�b�N�H
    void ShootingSystem(float elapsdTime);

    //�ˌ����̈ĎR�q�X�|�[��
    void ShootingSpawnCrow();

private:
    int     shootingScore = 0;                     //�ˌ��̃X�R�A
    int     scarecrowMaxTotal = 30;                //�ˌ����̍ő�ĎR�q�X�|�[����
    int     scarecrowCount = 0;                    //�ˌ����̈ĎR�q�X�|�[��
          
    float   scarecrowLifeTime = 1.0f;              //�X�|�[�����Ă���̐�������
    float   scarecrowLifeTimer = 0.0f;             //�������ԃ^�C�}�[
    float   scarecrowSpawnIntervalTime  = 1.0f;    //�ĎR�q���|����Ă���̎��̈ĎR�q���X�|�[���̊Ԋu
    float   scarecrowSpawnIntervalTimer = 0.0f;    //���̃^�C�}�[
    
    bool    shootingStartFlag = false;             //�ˌ����J�n���I�����@false=�I���@True���J�n
    bool    shootingIntervalFlag = false;          //���̈ĎR�q���X�|�[��������M��

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
};