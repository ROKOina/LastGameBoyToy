#pragma once

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

private:
    // �L�����ڍׁi�X�L���\���A�L�������A�I���L�����\���j
    void CharaDetails();

    // �L�����A�C�R���A���菈��
    

    // ���Ԑ���
    void TimeLimitSystem(float elapsedTime);

};

