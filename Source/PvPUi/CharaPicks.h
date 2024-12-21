#pragma once
#include <memory>

// �O���錾
class GameObject;
class Sprite;

class CharaPicks
{
public:
    CharaPicks() {}
    ~CharaPicks() {}

    // �L�����s�b�NUI����
    void CreateCharaPicksUiObject();
    // �X�V����
    void CharaPicksUpdate(float elapsedTime);

    // �L�����s�b�N�\���ݒ�
    void SetViewCharaPicks(bool flg);

    // �I�����ꂽ�L������ID���擾
    int GetSelectedCharacterId() const { return selectedCharacterId; }
private:
    // �L�����ڍׁi�X�L���\���A�L�������A�I���L�����\���j
    void CharaDetails();
    // ���菈��
    void DecisionButton();
    // ���Ԑ���
    void TimeLimitSystem(float elapsedTime);

private:

    struct CharacterInfo {
        std::shared_ptr<GameObject> chara;
        std::shared_ptr<Sprite> sprite;
        std::shared_ptr<GameObject> name;
        std::shared_ptr<GameObject> skill;
        int id;
    };

    // �I�����ꂽ�L������ID
    int selectedCharacterId = -1;
    // ����t���O
    bool decisionFlg = false;
};

