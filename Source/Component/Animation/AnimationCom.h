#pragma once

#include <DirectXMath.h>

#include "Component\System\Component.h"
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

    struct PlayLowBodyAnimParam
    {
        int   lowerAnimaOneId; //再生したいアニメーションを入れて下さい
        int   lowerAnimeTwoId = 0;  ///歩きモーション用
        int   lowerAnimeThreeId = 0;///歩きモーション用
        int   lowerAnimeFourId = 0; ///歩きモーション用
        bool  loop = false;         //ループ再生するかしないか
        bool  rootFlag = false;     //ルートモーションするかしないか
        int   blendType = 0;        // 0=ノーマルアニメーション 1=ブレンドアニメーション 2=歩きブレンドアニメーション
        float animeChangeRate = 0.0f; //アニメーション切り替え時のアニメーション変更速度
        float animeBlendRate = 0.0f;  //ブレンドの利率
    };

public:
    //アニメーション再生関数

    //普通のアニメーション再生関数
    void PlayAnimation(int animeID, bool loop, bool rootFlag = false, float blendSeconds = 0.25f);
    //上半身だけアニメーション再生
    void PlayUpperBodyOnlyAnimation(int upperAnimaId, bool loop, float blendSeconds);
    //下半身だけアニメーション再生 lowerAnimeTwoIdはブレンドアニメーションしないときは‐1を入れといて下さい blendType　 0=ノーマルアニメーション 1=ブレンドアニメーション 2=歩きブレンドアニメーション
    void PlayLowerBodyOnlyAnimation(PlayLowBodyAnimParam param);
    //再生中か
    bool IsPlayAnimation() const { return currentAnimation >= 0; }
    //上半身アニメーション再生中か？
    bool IsPlayUpperAnimation();
    //下半身アニメーション再生中か？
    bool IsPlayLowerAnimation();

    //アニメーションストップ
    void StopAnimation();

    //アニメーション一時停止
    void StopOneTimeAnimation() { oneTimeStop = true; }

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

    //アニメーション更新切り替え
    void SetUpAnimationUpdate(int updateId);

    //アニメーションイベント呼び出し（true:イベント中）
    bool IsEventCalling(std::string eventName);
    //イベント中に指定されたノードのワールド位置を送る
    bool IsEventCallingNodePos(std::string eventName, std::string nodeName, DirectX::XMFLOAT3& pos);

private:

    //下半身別アニメーション計算
    void ComputeLowerAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, float blendRate, float walkBlend, int index);
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
    int                             lowerAnimationTwoIndex = 0;
    int                             lowerAnimationThreeIndex = 0;
    int                             lowerAnimationFourIndex = 0;

    //下半身のアニメーション制御の番号 0=normal 1=blendAnime 2=walkAnime
    int                             lowerBlendType = 0;

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
    //上半身下半身ブレンドアニメーション管理フラグ
    bool                            upperBlendTypeFlag = false;
    bool                            lowerBlendTypeFlag = false;

    //アニメーション更新変数
    int animaType = 0;

    //上半身アニメーション
    std::vector<Model::Node*> upperNodes;
    //下半身アニメーション
    std::vector<Model::Node*> lowerNodes;

    //////分離/////////////

    //アニメーションイベント関連
    void AnimEventWindow();
    bool isEventWindow = false; //イベントウィンドウ表示
    float animEventSeconds = 0; //イベント用のアニメーション時間
    bool oneTimeStop = false;   //一時停止用
};