#pragma once
#include "CharacterCom.h"
#include "Components/GPUParticle.h"

class UenoCharacterCom : public CharacterCom
{
public:
    UenoCharacterCom() {};
    ~UenoCharacterCom() {}

    // –¼‘Oæ“¾
    const char* GetName() const override { return "UenoCharacter"; }

    //‰Šú‰»
    void Start() override;

    //XVˆ—
    void Update(float elapsedTime) override;

    //imgui
    void OnGUI() override;

    void MainAttackPushing() override;

    void SubAttackPushing() override;

    void SubSkill() override;

    void SpaceSkill() override;

    void LeftShiftSkill() override;
};