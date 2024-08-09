#pragma once
#include "CharacterCom.h"
#include "Components/GPUParticle.h"

class UenoCharacterCom : public CharacterCom
{
public:
    UenoCharacterCom();
    ~UenoCharacterCom() {}

    // 名前取得
    const char* GetName() const override { return "UenoCharacter"; }

    //初期化
    void Start() override;

    //更新処理
    void Update(float elapsedTime) override;

    //imgui
    void OnGUI() override;

    //メインアタック
    void MainAttackDown()override;

public:

    void SetLazerFlag(const bool& lazer) { lazerflag = lazer; }
    const bool& GetLazerFlag()const { return lazerflag; }

public:

    std::shared_ptr<GPUParticle>gpulazerparticle;

private:

    //パーティクル更新
    void LazerParticleUpdate(float elapsedTime);

private:
    bool lazerflag = false;
};