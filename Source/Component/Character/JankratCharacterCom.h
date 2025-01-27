#pragma once
#include "CharacterCom.h"
#include "Audio\Audio3D.h"

class JankratCharacterCom : public CharacterCom
{
public:
    ~JankratCharacterCom() override;

    // 名前取得
    const char* GetName() const override { return "JankratCharacter"; }

    void Start() override;

    void Update(float elapsedTime) override;

    //通常攻撃
    void MainAttackDown() override;

    //Eskill
    void SubAttackDown() override;
    void SubSkill() override;

    //ult
    void UltSkill() override;

private:

    //消去されている弾や地雷を手放す
    void EraseHaveObjects();

    //銃の打つ間隔とマゼルフラッシュ
    void ShotSecond();

public:
    std::weak_ptr<GameObject> GetHaveBullet() { return haveBullet; }
    void SetHaveBullet(std::weak_ptr<GameObject> obj) { haveBullet = obj; }
    void ReleaseHaveBullet() { haveBullet.reset(); }

    std::vector<std::weak_ptr<GameObject>> GetHaveMine() { return haveMine; }
    void AddHaveMine(std::weak_ptr<GameObject> obj) { haveMine.emplace_back(obj); }
    void ReleaseHaveMine(std::weak_ptr<GameObject> obj)
    {
        haveMine.erase(
            std::remove_if(
                haveMine.begin(),
                haveMine.end(),
                [&obj](const std::weak_ptr<GameObject>& mine) {
                    // ロックして比較
                    return !mine.owner_before(obj) && !obj.owner_before(mine);
                }
            ),
            haveMine.end()
        );
    }
    void AllReleaseHaveMine()
    {
        for (auto& mine : haveMine)
        {
            if (auto sharedMine = mine.lock())
            {
                sharedMine.reset(); // 共有所有権をリセット
            }
        }
        haveMine.clear();
    }

private:

    std::weak_ptr<GameObject> haveBullet; //射撃待機中の弾丸
    std::vector<std::weak_ptr<GameObject>> haveMine; //使用中の地雷

private:
    std::map<std::string, GameObj> audioObjs;
public:
    AudioSource3D* GetAudio(std::string name) { return audioObjs[name]->GetComponent<AudioSource3D>().get(); }
};
