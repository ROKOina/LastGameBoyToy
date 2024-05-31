#pragma once

#include <DirectXMath.h>

#include "System\Component.h"

#include "Graphics\Model\Model.h"

//アニメーション
class AnimationCom : public Component
{
    // コンポーネントオーバーライド
public:
    AnimationCom() {}
    ~AnimationCom() {}

    // 名前取得
    const char* GetName() const override { return "Animation"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    // アニメーション更新
    void AnimationUpdata(float elapsedTime);

    //Animationクラス
public:

private:
};