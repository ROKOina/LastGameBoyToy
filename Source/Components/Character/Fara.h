#pragma once

#include "CharacterCom.h"
#include "Components/GPUParticle.h"

class Fara : public CharacterCom
{
public:
    Fara() {};
    ~Fara() {}

    // –¼‘Oæ“¾
    const char* GetName() const override { return "Fara"; }

    //‰Šú‰»
    void Start() override;

    //XVˆ—
    void Update(float elapsedTime) override;

    //imgui
    void OnGUI() override;
};