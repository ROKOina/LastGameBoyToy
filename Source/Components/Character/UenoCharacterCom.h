#pragma once
#include "CharacterCom.h"
#include "Components/GPUParticle.h"

class UenoCharacterCom : public CharacterCom
{
public:
    UenoCharacterCom() {};
    ~UenoCharacterCom() {}

    // 名前取得
    const char* GetName() const override { return "UenoCharacter"; }

    //初期化
    void Start() override;

    //更新処理
    void Update(float elapsedTime) override;

    //imgui
    void OnGUI() override;

    //左クリック
    void MainAttackPushing() override;
};