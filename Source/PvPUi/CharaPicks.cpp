#include "CharaPicks.h"
#include "Component\System\GameObject.h"
#include "Component/Sprite/Sprite.h"

// �L�����s�b�NUI����
void CharaPicks::CreateCharaPicksUiObject()
{
    // �L�����s�b�N�L�����p�X
    auto& charaPicksCanvas = GameObjectManager::Instance().Create();
    charaPicksCanvas->SetName("CharaPicksCanvas");

    // �e�L������UI�ݒ�
    {
        // INAZAWA
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("INAZAWA");
        }
        // FARAH
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("FARAH");
        }
        // JANKRAT
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("JANKRAT");
        }
        // 4
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("chara4");
        }
    }

    // ����{�^��
    {
        auto& decision = charaPicksCanvas->AddChildObject();
        decision->SetName("decision");
    }

    // ���Ԑ���
    {
        auto& timeLimit = charaPicksCanvas->AddChildObject();
        timeLimit->SetName("TimeLimit");
    }
}

// �L�����s�b�N�X�V����
void CharaPicks::CharaPicksUpdate(float elapsedTime)
{
    // �e�L�����ڍ�
    CharaDetails();
    
    // ���Ԑ����V�X�e��
    TimeLimitSystem(elapsedTime);
}

// �L�����ڍ�
void CharaPicks::CharaDetails()
{
    // �A�C�R����������
    // �N���b�N����ƃX�L���\���A�L�������A�I���L�����A�A�C�R�����\��
}

// ���菈��

// ���Ԑ����V�X�e��
void CharaPicks::TimeLimitSystem(float elapsedTime)
{
    // �������Ԃ͐ݒ�ł���悤��
    // �������Ԃ���A�Ȃ��؂�ւ����悤�ɂ���
}

// �L�����s�b�N�\���ݒ�
void CharaPicks::SetViewCharaPicks(bool flg)
{
    auto& charaPicksCanvas = GameObjectManager::Instance().Find("CharaPicksCanvas");
    if (!charaPicksCanvas) return;

    if (flg)
        charaPicksCanvas->SetEnabled(true);
    else
        charaPicksCanvas->SetEnabled(false);
}