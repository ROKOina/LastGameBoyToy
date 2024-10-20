#pragma once

#include "CharacterCom.h"
#include "Components/GPUParticle.h"

class Fara : public CharacterCom
{
public:
    Fara() {};
    ~Fara() {}

    // 名前取得
    const char* GetName() const override { return "Fara"; }

    //初期化
    void Start() override;

    //更新処理
    void Update(float elapsedTime) override;

    //imgui
    void OnGUI() override;
};