#pragma once

#include "../System\Component.h"

class OnGroundDeleteCom : public Component
{
public:
    OnGroundDeleteCom() {};
    ~OnGroundDeleteCom() override {};

    // 名前取得
    const char* GetName() const override { return "OnGroundDeleteCom"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

private:

    //ステージに当たれば削除する関数
    void IsGroundDelete();

    //ヒットした位置と法線を取得して設定
    void IsRayCast();
};