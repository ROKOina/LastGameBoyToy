#pragma once

#include <memory>

class GameObject;
class RegisterChara
{
public:
    RegisterChara() {}
    ~RegisterChara() {}

    // インスタンス取得
    static RegisterChara& Instance()
    {
        static RegisterChara instance;
        return instance;
    }

    enum class CHARA_LIST
    {
        INAZAWA,
        HAVE_ALL_ATTACK,
        FARAH,
        JANKRAT,
        MAX,
    };

    //キャラに必要なコンポーネントを追加する
    void SetCharaComponet(CHARA_LIST list, std::shared_ptr<GameObject>& obj);

    //imgui
    void ImGui();

private:
    void InazawaChara(std::shared_ptr<GameObject>& obj);
    void HaveAllAttackChara(std::shared_ptr<GameObject>& obj);
    void FarahCharacter(std::shared_ptr<GameObject>& obj);
    void JankratChara(std::shared_ptr<GameObject>& obj);


};