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

    //上半身別アニメーション更新
    void AnimationSortingUpdate(float elapsedTime);

    //Animationクラス
public:
    //アニメーション再生関数

    //普通のアニメーション再生関数
    void PlayAnimation(int animeID, bool loop, bool rootFlag = false, float blendSeconds = 0.25f);
    //再生中か
    bool IsPlayAnimation() const { return currentAnimation >= 0; }
    //アニメーションストップ
    void StopAnimation();

    //アニメIDを返す
    int FindAnimation(const char* animeName);

public:
    //ゲッターセッター

    // アニメーション時間
    float GetAnimationSeconds() const { return currentSeconds; }
    void SetAnimationSeconds(float seconds) { currentSeconds = seconds; }

    //ルートモーションの値を取るノードを検索
    void SetupRootMotion(const char* rootMotionNodeName);
    //ルートモーションの腰を取るノードを検索
    void SetupRootMotionHip(const char* rootMotionNodeName);
private:

    //アニメーション計算
    void ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Model::Node& node);
    //アニメーション切り替え時の計算
    void ComputeSwitchAnimation(const ModelResource::NodeKeyData& key1, const float blendRate, Model::Node& node);

private:

    //ルートモーション関連

    //ルートモーション更新
    void updateRootMotion(DirectX::XMFLOAT3& translation);
    //ルートモーションの移動値を計算
    void ComputeRootMotion();

private:
    //変数

    //ノーマルアニメーション

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

    //ルートモーション

    //動かすノードのインデックス
    int rootMotionNodeIndex = -1;
    //ポジションを入れるノードのインデックス
    int rootMotionHipNodeIndex = -1;
    //ルートモーションするかどうか
    bool                            rootFlag = false;
    //ルートモーション　アニメーション更新処理判定
    bool                            rootMotionFlag = false;
    //ルートモーションで使う値を保存しておく変数
    DirectX::XMFLOAT3               cahcheRootMotionTranslation = { 0,0,0 };
    //フレーム間の差分用変数
    DirectX::XMFLOAT3               rootMotionTranslation = { 0,0,0 };


    //上半身関連

    //現在の再生している上半身アニメーション番号
    int                             currentUpperAnimationIndex = -1;
    //上半身のアニメーション番号
    int                             upperAnimationIndex = 0;
    //上半身アニメーションを変更した時の変数
    float                           upperAnimationChangeRate = 0.0f;
    //上半身アニメーションを変更時の現在の時間
    float                           upperAnimationChangeTime = 0.0f;
    //上半身のアニメーションの長さ
    float                           upperAnimationSecondsLength = 0.0f;
    //上半身アニメーションの現在の時間
    float                           upperCurrentAnimationSeconds = 0.0f;





    //上半身のアニメーション再生中か？
    bool                            upperAnimaPlay = false;
    //上半身のアニメーション再生中か 再生中=true 再生していない=false
    bool                            upperIsPlayAnimation = false;
    //上半身アニメーションが終了しているか
    bool                            animationUpperEndFlag = false;
    //上半身アニメーションがループしているか
    bool                            animationUpperLoopFlag = false;
    //アニメーションの終わる1フレーム前に立つフラグ
    bool                            beforeOneFream = false;
    //上半身終わったらのフラグ 終了＝true 
    bool                            upperAnimationEndFlag = false;
    //上半身補完フラグ
    bool                            upperComplementFlag = false;

    //アニメーション更新タイプ
    enum class AnimationType
    {
        NormalAnimation,
        UpperLowerAnimation,
        UpperBlendLowerAnimation,
    };

};