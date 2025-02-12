#pragma once

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
        FARAH,
        JANKRAT,
        SOLIDER,
        MAX,
    };

    //キャラに必要なコンポーネントを追加する
    void SetCharaComponet(CHARA_LIST list, std::shared_ptr<GameObject>& obj, bool myTeam);

    //キャラ変更
    void ChangeChara(std::string objName, CHARA_LIST list); //名前は今のオブジェクトの名前

private:
    void InazawaChara(std::shared_ptr<GameObject>& obj, bool myTeam);
    void FarahCharacter(std::shared_ptr<GameObject>& obj, bool myTeam);
    void JankratChara(std::shared_ptr<GameObject>& obj, bool myTeam);
    void SoldireChar(std::shared_ptr<GameObject>& obj, bool myTeam);
};