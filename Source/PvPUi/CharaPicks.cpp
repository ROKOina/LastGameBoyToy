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
                name->SetEnabled(false);
            }


            // �X�L���\�L
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill1.ui", Sprite::SpriteShader::DEFALT, false);
                skill->SetEnabled(false);
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
                name->SetEnabled(false);
            }

            // �X�L���\�L
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill2.ui", Sprite::SpriteShader::DEFALT, false);
                skill->SetEnabled(false);
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
                name->SetEnabled(false);
            }

            // �X�L���\�L
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill3.ui", Sprite::SpriteShader::DEFALT, false);
                skill->SetEnabled(false);
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
    // �N���b�N����ƃX�L���\���A�L�������A�I���L�����A�A�C�R�����\��
    GamePad& gamePad = Input::Instance().GetGamePad();

    // �A�C�R����I��������\��
    auto& canvas = GameObjectManager::Instance().Find("CharaPicksCanvas");

    auto getCharacterInfo = [&](const std::string& charaName, int id) -> CharacterInfo {
        auto& chara = canvas->GetChildFind(charaName.c_str());
        return {
            chara,
            chara->GetComponent<Sprite>(),
            chara->GetChildFind("name"),
            chara->GetChildFind("skill"),
            id
        };
        };

    // �L�����AID�ݒ�
    std::vector<CharacterInfo> characters = {
        getCharacterInfo("INAZAWA", 0),
        getCharacterInfo("FARAH", 1),
        getCharacterInfo("JANKRAT", 2),
        getCharacterInfo("chara4", 3)
    };

    auto handleCharacterSelection = [&](CharacterInfo& selected, std::vector<CharacterInfo>& others) {
        if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown() && selected.sprite->GetHitSprite()) {
            selected.name->SetEnabled(true);
            // �X�L���\���͂����̃R�����g�������ĂˁI
            //selected.skill->SetEnabled(true);
            selectedCharacterId = selected.id;

            for (auto& other : others) {
                if (&other != &selected) {
                    other.name->SetEnabled(false);
                    other.skill->SetEnabled(false);
                }
            }
        }
        };

    for (size_t i = 0; i < characters.size(); ++i) {
        std::vector<CharacterInfo> others = characters;
        others.erase(others.begin() + i);
        handleCharacterSelection(characters[i], others);
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