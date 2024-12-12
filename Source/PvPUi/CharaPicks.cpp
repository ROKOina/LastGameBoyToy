#include "CharaPicks.h"
#include "Input/Input.h"

#include "Component/Sprite/Sprite.h"
#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"

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
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon0.ui", Sprite::SpriteShader::DEFALT, true);

            // ���O�\�L
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName0.ui", Sprite::SpriteShader::DEFALT, false);
                name->SetEnabled(false);
            }

            // �X�L���\�L
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill0.ui", Sprite::SpriteShader::DEFALT, false);
                skill->SetEnabled(false);
            }


        }
        // FARAH
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("FARAH");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon1.ui", Sprite::SpriteShader::DEFALT, true);

            // ���O�\�L
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName1.ui", Sprite::SpriteShader::DEFALT, false);
                name->GetComponent<Sprite>()->SetEnabled(false);
            }


            // �X�L���\�L
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill1.ui", Sprite::SpriteShader::DEFALT, false);
                skill->GetComponent<Sprite>()->SetEnabled(false);
            }

        }
        // JANKRAT
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("JANKRAT");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon2.ui", Sprite::SpriteShader::DEFALT, true);

            // ���O�\�L
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName2.ui", Sprite::SpriteShader::DEFALT, false);
                name->GetComponent<Sprite>()->SetEnabled(false);
            }

            // �X�L���\�L
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill2.ui", Sprite::SpriteShader::DEFALT, false);
                skill->GetComponent<Sprite>()->SetEnabled(false);
            }

        }
        // 4
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("chara4");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon3.ui", Sprite::SpriteShader::DEFALT, true);

            // ���O�\�L
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName3.ui", Sprite::SpriteShader::DEFALT, false);
                name->GetComponent<Sprite>()->SetEnabled(false);
            }

            // �X�L���\�L
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill3.ui", Sprite::SpriteShader::DEFALT, false);
                skill->GetComponent<Sprite>()->SetEnabled(false);
            }

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
    GamePad& gamePad = Input::Instance().GetGamePad();

    // �A�C�R���ɐG��Ă�����\��
    {
        auto& canvas = GameObjectManager::Instance().Find("CharaPicksCanvas");

        {
            auto& chara = canvas->GetChildFind("INAZAWA");
            auto& sprite = chara->GetComponent<Sprite>();

            auto& name = chara->GetChildFind("name");
            auto& skill = chara->GetChildFind("skill");

            // �X�v���C�g����
            if (sprite->GetHitSprite())
            {
                name->SetEnabled(true);
                skill->SetEnabled(true);
            }
            else
            {
                name->SetEnabled(false);
                skill->SetEnabled(false);
            }
        }
    }


    if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
    {

    }
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