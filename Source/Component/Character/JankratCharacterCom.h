#pragma once
#include "CharacterCom.h"

class JankratCharacterCom : public CharacterCom
{
public:
    // ���O�擾
    const char* GetName() const override { return "JankratCharacter"; }

    void Start() override;

    void Update(float elapsedTime) override;

    void MainAttackDown() override;

    void SubAttackDown() override;
    void SubSkill() override;

    void Reload() override;

    void UltSkill() override {};

private:
    //��������Ă���e��n���������
    void EraseHaveObjects();

    //�e�̑łԊu�ƃ}�[���t���b�V��
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

    GameObj haveBullet; //�ˌ��ҋ@���̒e��
    std::vector<GameObj> haveMine; //�g�p���̒n��

    int maxBulletNum = 8;
    int maxMineNum = 2;
};
