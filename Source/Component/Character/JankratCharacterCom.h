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
    void MainSkill() override;

    void SubAttackDown() override;
    void SubSkill() override {};

    void UltSkill() override {};

    void ResetShootTimer() { shootTimer = 0; }


private:
    //消去されている弾や地雷を手放す
    void EraseHaveObjects();

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
    //発射間隔
    float shootTime = 0.6f;
    float shootTimer = 0;

    GameObj haveBullet; //射撃待機中の弾丸
    std::vector<GameObj> haveMine; //使用中の地雷
};
