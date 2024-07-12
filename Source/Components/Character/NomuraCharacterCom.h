#pragma once
#include "CharacterCom.h"

class NomuraCharacterCom :public CharacterCom
{
public:
    //名前取得
    const char* GetName()const override { return "NomuraCharacter"; }

    void Start() override;

    void Update(float elapsedTime)override;

    void OnGUI() override;

    void MainAttack()override;

    void SubAttack()override;

    void SubSkill()override;

    void UltSkill()override;

    //void SpaceSkill()override;

public:
    //ゲッターセッター その他
    int GetNowMagazin() { return nowMagazine; }

    void MinusMagazin() { nowMagazine -= 1; }

public:

    void Reload();

private:

    void FireInterval(float elapsedTime);

    void ReloadManagement();

    void BulletManagement();
private:


    int   nowMagazine = 6;
    int   maxBullets = 6;
    float attackPower = 0;
    float maxAttackPower = 1;
    float bulletSpeed = 50;
    float firingInterval = 0.3f;
    float firingIntervalTimer = 0.3f;



    bool  launchPermission = true;
    bool  rapidLaunchPermission = true;


    bool  remainingBullets = true;
    bool  rapidRemainingBullets = true;
   
    //ダッシュ
    bool dashFlag = false;

};