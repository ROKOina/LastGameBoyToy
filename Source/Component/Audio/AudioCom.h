#pragma once
#include "../System\Component.h"
#include ".//"

class AudioCom : public Component
{
public:
    AudioCom() {};
    ~AudioCom() override {};

    // 名前取得
    const char* GetName() const override { return "Audio"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

private:
};
