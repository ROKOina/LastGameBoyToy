#pragma once
#include "CharacterCom.h"

class JankratCharacterCom : public CharacterCom
{
public:
    // 名前取得
    const char* GetName() const override { return "JankratCharacter"; }

    void Start() override;

    void Update(float elapsedTime) override;

    void MainAttackDown() override;

    void SubAttackDown() override;
    void SubSkill() override;

    void Reload() override;

    void UltSkill() override {};

private:
    //消去されている弾や地雷を手放す
    void EraseHaveObjects();

    //銃の打つ間隔とマゼルフラッシュ
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

    GameObj haveBullet; //射撃待機中の弾丸
    std::vector<GameObj> haveMine; //使用中の地雷

    int maxBulletNum = 8;
    int maxMineNum = 2;
};
