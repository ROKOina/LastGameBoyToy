#pragma once

#include <memory>

class GameObject;

class RegisterChara
{
public:
    RegisterChara() {}
    ~RegisterChara() {}

    // �C���X�^���X�擾
    static RegisterChara& Instance()
    {
        static RegisterChara instance;
        return instance;
    }

    enum class CHARA_LIST
    {
        INAZAWA,
        HAVE_ALL_ATTACK,
        PICOHARD,
        JANKRAT,
        MAX,
    };

    //�L�����ɕK�v�ȃR���|�[�l���g��ǉ�����
    void SetCharaComponet(CHARA_LIST list, std::shared_ptr<GameObject>& obj);

private:
    void InazawaChara(std::shared_ptr<GameObject>& obj);
    void HaveAllAttackChara(std::shared_ptr<GameObject>& obj);
    void UenoChara(std::shared_ptr<GameObject>& obj);
    void PicohardChara(std::shared_ptr<GameObject>& obj);
    void JankratChara(std::shared_ptr<GameObject>& obj);
};