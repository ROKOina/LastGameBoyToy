#pragma once

#include <DirectXMath.h>

#include "System\Component.h"
#include "Graphics/Model/Model.h"
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
    //アニメーション再生関数

    //普通のアニメーション再生関数
    void PlayAnimation(int animeID,bool loop,float blendSeconds);
    //再生中か
    bool IsPlayAnimation() const { return currentAnimation >= 0; }
    //アニメーションストップ
    void StopAnimation();

public:
    // アニメーション時間
    float GetAnimationSeconds() const { return currentSeconds; }
    void SetAnimationSeconds(float seconds) { currentSeconds = seconds; }

private:

    //アニメーション計算
    void ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Model::Node&node);

private:
    //変数

    //現在のアニメーションID
    int								currentAnimation = -1;
    //アニメーションの現在の時間
    float							currentSeconds = 0.0f;
    //アニメーションを変更した時の変数
    float                           animationChangeRate = 0.0f;
    //アニメーションを変更時の現在の時間
    float                           animationChangeTime = 0.0f;
    //アニメーションをループさせるか？
    bool							loopAnimation = false;
    //アニメーションを終了させるか？
    bool							endAnimation = false;

};