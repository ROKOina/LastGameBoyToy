#pragma once

#include <DirectXMath.h>

#include "System\Component.h"
#include "Graphics/Model/Model.h"
#include "Graphics\Model\Model.h"
#include "Graphics/Model/ModelResource.h"

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
    void AnimationUpperUpdate(float elapsedTime);
    //下半身別アニメーション更新
    void AnimationLowerUpdate(float elapsedTime);
    //Animationクラス

public:
    //アニメーション更新タイプ
    enum  AnimationType
    {
        NormalAnimation,
        UpperLowerAnimation,
        UpperBlendLowerAnimation,
    };

public:
    //アニメーション再生関数

    //普通のアニメーション再生関数
    void PlayAnimation(int animeID, bool loop, bool rootFlag = false, float blendSeconds = 0.25f);
    //上半身だけアニメーション再生
    void PlayUpperBodyOnlyAnimation(int upperAnimaId, bool loop, float blendSeconds);
    //下半身だけアニメーション再生
    void PlayLowerBodyOnlyAnimation(int lowerAnimaId, bool loop, bool rootFlag, float blendSeconds);
    //再生中か
    bool IsPlayAnimation() const { return currentAnimation >= 0; }
    //上半身アニメーション再生中か？
    bool IsPlayUpperAnimation();
    //下半身アニメーション再生中か？
    bool IsPlayLowerAnimation();

    //アニメーションストップ
    void StopAnimation();

    //アニメIDを返す
    int FindAnimation(const char* animeName);

public:
    //ゲッターセッター

    // アニメーション時間
    float GetAnimationSeconds() const { return currentSeconds; }
    void SetAnimationSeconds(float seconds) { currentSeconds = seconds; }

    // 現在のアニメーション番号取得
    int GetCurrentAnimationIndex()const { return currentAnimation; }
    int GetCurrentUpperAnimationIndex()const { return currentUpperAnimation; }
    int GetCurrentLowerAnimationIndex()const { return currentLowerAnimation; }

    //ルートモーションの値を取るノードを検索
    void SetupRootMotion(const char* rootMotionNodeName);
    //ルートモーションの腰を取るノードを検索
    void SetupRootMotionHip(const char* rootMotionNodeName);
    //ルートモーション更新
    void updateRootMotion(DirectX::XMFLOAT3& translation);
    //アニメーション更新切り替え
    void SetUpAnimationUpdate(int updateId);
private:

    //アニメーション計算
    void ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Model::Node& node);
    //アニメーション切り替え時の計算
    void ComputeSwitchAnimation(const ModelResource::NodeKeyData& key1, const float blendRate, Model::Node& node);

    //AimIK関数
    void AimIK();

    //ノードを探す
    void SearchAimNode();

    //上半身と下半身のノードを分ける
    void SeparateNode();

private:

    //ルートモーション関連

    //ルートモーションの移動値を計算
    void ComputeRootMotion();

private:

    //Debug用選択アニメーションを取得
    ModelResource::Animation* GetSelectionAnimation();

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

    //上半身下半身関連

    //現在の再生している上半身下半身アニメーション番号
    int                             currentUpperAnimation = -1;
    int                             currentLowerAnimation = -1;
    //上半身下半身のアニメーション番号
    int                             upperAnimationIndex = 0;
    int                             lowerAnimationIndex = 0;
    //上半身のノードの番号
    int                             upperID = 2;
    //下半身ノードの番号
    int                             lowerID = 1;
    //上半身下半身アニメーションを変更した時の変数
    float                           upperAnimationChangeRate = 0.0f;
    float                           lowerAnimationChangeRate = 0.0f;
    //上半身下半身アニメーションを変更時の現在の時間
    float                           upperAnimationChangeTime = 0.0f;
    float                           lowerAnimationChangeTime = 0.0f;
    //上半身下半身のアニメーションの長さ
    float                           upperAnimationSecondsLength = 0.0f;
    float                           lowerAnimationSecondsLength = 0.0f;
    //上半身下半身アニメーションの現在の時間
    float                           upperCurrentAnimationSeconds = 0.0f;
    float                           lowerCurrentAnimationSeconds = 0.0f;
    //上半身下半身アニメーションブレンドのブレンドレート
    float                           upperBlendRate = 0.0f;
    float                           lowerBlendRate = 0.0f;
    //上半身下半身アニメーションのブレンドレート
    float                           upperRate = 0.0f;
    float                           lowerRate = 0.0f;

    //上半身下半身のアニメーション再生中か？
    bool                            upperAnimaPlay = false;
    bool                            lowerAnimaPlay = false;
    //上半身下半身のアニメーション再生中か 再生中=true 再生していない=false
    bool                            upperIsPlayAnimation = false;
    bool                            lowerIsPlayAnimation = false;
    //上半身下半身アニメーションが終了しているか
    bool                            animationUpperEndFlag = false;
    bool                            animationLowerEndFlag = false;
    //上半身下半身アニメーションがループしているか
    bool                            animationUpperLoopFlag = false;
    bool                            animationLowerLoopFlag = false;
    //アニメーションの終わる1フレーム前に立つフラグ
    bool                            beforeOneFream = false;
    //上半身下半身終わったらのフラグ 終了＝true
    bool                            upperAnimationEndFlag = false;
    bool                            lowerAnimationEndFlag = false;
    //上半身下半身補完フラグ
    bool                            upperComplementFlag = false;
    bool                            lowerComplementFlag = false;

    //アニメーション更新変数
    int animaType = 0;

    //AimIK用変数
    std::vector<int>AimBone;

    //上半身アニメーション
    std::vector<Model::Node*> upperNodes;
    //下半身アニメーション
    std::vector<Model::Node*> lowerNodes;
};