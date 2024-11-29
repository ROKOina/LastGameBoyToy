#pragma once
#include "CharacterCom.h"

class JankratCharacterCom : public CharacterCom
{
public:
    // –¼‘OŽæ“¾
    const char* GetName() const override { return "JankratCharacter"; }

    void Start() override;

    void Update(float elapsedTime) override;

    void OnGUI() override {};

    void MainAttackDown() override;

    void SubAttackDown() override {};
    void SubSkill() override {};

    void UltSkill() override {};

    void ResetShootTimer() { shootTimer = 0; }

    GameObj GetHaveBullet() { return haveBullet; }
    void SetHaveBullet(GameObj obj) { haveBullet = obj; }
    void haveBulletRelease() { haveBullet.reset(); }

private:
    //”­ŽËŠÔŠu
    float shootTime = 0.6f;
    float shootTimer = 0;

    GameObj haveBullet; //ŽËŒ‚‘Ò‹@’†‚Ì’eŠÛ
};
