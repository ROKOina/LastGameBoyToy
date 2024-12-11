#pragma once
#include "CharacterCom.h"

class JankratCharacterCom : public CharacterCom
{
public:
    ~JankratCharacterCom() override;

    // –¼‘Oæ“¾
    const char* GetName() const override { return "JankratCharacter"; }

    void Start() override;

    void Update(float elapsedTime) override;

    //’ÊíUŒ‚
    void MainAttackDown() override;

    //Eskill
    void SubAttackDown() override;
    void SubSkill() override;

    //ult
    void UltSkill() override;

private:
    //Á‹‚³‚ê‚Ä‚¢‚é’e‚â’n—‹‚ğè•ú‚·
    void EraseHaveObjects();

    //e‚Ì‘Å‚ÂŠÔŠu‚Æƒ}ƒ[ƒ‹ƒtƒ‰ƒbƒVƒ…
    void ShotSecond();

public:
    GameObj GetHaveBullet() { return haveBullet; }
    void SetHaveBullet(GameObj obj) { haveBullet = obj; }
    void ReleaseHaveBullet() { haveBullet.reset(); }

    std::vector<GameObj> GetHaveMine() { return haveMine; }
    void AddHaveMine(GameObj obj) { haveMine.emplace_back(obj); }
    void ReleaseHaveMine(GameObj obj) { haveMine.erase(std::remove(haveMine.begin(), haveMine.end(), obj), haveMine.end()); }
    void AllReleaseHaveMine()
    {
        for (auto& mine : haveMine)
        {
            mine.reset();
        }
        haveMine.clear();
    }

private:

    GameObj haveBullet; //ËŒ‚‘Ò‹@’†‚Ì’eŠÛ
    std::vector<GameObj> haveMine; //g—p’†‚Ì’n—‹

    int maxBulletNum = 8;
    int maxMineNum = 2;
};
