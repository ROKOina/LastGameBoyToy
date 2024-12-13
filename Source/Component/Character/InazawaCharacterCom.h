#pragma once
#include "CharacterCom.h"

class InazawaCharacterCom : public CharacterCom
{
public:
    // 名前取得
    const char* GetName() const override { return "InazawaCharacter"; }

    void Start() override;

    void Update(float elapsedTime) override;

    void OnGUI() override;

    void MainAttackDown() override;

    void SubAttackDown() override;
    void SubSkill() override;

    void UltSkill() override;

    int* GetRCounter() { return &attackUltCounter; }
    int GetRMaxCount() { return attackUltCountMax; }

    //攻撃ウルト取得
    void SetAttackUltRayObj(std::shared_ptr<GameObject> obj) { attackUltRayObj = obj; }

    //リロード（弾減らす処理は各自のキャラでする
    void Reload()override;

private:

    //アタックULT
    int attackUltCountMax = 5;  //ウルトを打てる数
    int attackUltCounter;
    std::weak_ptr<GameObject> attackUltRayObj;  //ウルトレイ
};
