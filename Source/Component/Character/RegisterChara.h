#pragma once

#include <memory>
#include <string>

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
        //HAVE_ALL_ATTACK,
        FARAH,
        JANKRAT,
        MAX,
    };

    //�L�����ɕK�v�ȃR���|�[�l���g��ǉ�����
    void SetCharaComponet(CHARA_LIST list, std::shared_ptr<GameObject>& obj);

    //�L�����ύX
    void ChangeChara(std::string objName, CHARA_LIST list); //���O�͍��̃I�u�W�F�N�g�̖��O

    //imgui
    void ImGui();

private:
    void InazawaChara(std::shared_ptr<GameObject>& obj);
    void HaveAllAttackChara(std::shared_ptr<GameObject>& obj);
    void FarahCharacter(std::shared_ptr<GameObject>& obj);
    void JankratChara(std::shared_ptr<GameObject>& obj);


};