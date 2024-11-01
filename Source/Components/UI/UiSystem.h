#pragma once
#include "Graphics\Sprite\Sprite.h"
#include "math.h"
class UiSystem : public Sprite
{
    //コンポーネントオーバーライド
public:
    UiSystem(const char* filename, SpriteShader spriteshader, bool collsion);
    ~UiSystem() {}

    // 名前取得
    const char* GetName() const override { return "UiSystem"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    //描画
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // GUI描画
    void OnGUI() override;


    void FadeIn(float elapsedTime);
    void FadeOut(float elapsedTime);


    //セッター,ゲッター
    void SetVariableValue(float* value) { variableValue = value; }
    void SetMaxValue(float value) { maxValue = value; }
    void SetTImer(float value) { timer = value; }
    void SetFadeTimer(float value) { fadeTimer = value; originalFadeTime = value; }
    void SetFadeInFlag(bool flag) { fadeInFlag = flag;}
    void SetFadeOutFlag(bool flag) { fadeOutFlag = flag;}


protected:
    float* variableValue;    //参照する値
    float  maxValue = 0.0f;         //最大値
    float  valueRate = 0.0f;        //割合

    float timer;            //汎用タイマー

    float fadeTimer = 0.0f;        //フェード用タイマー
    float originalFadeTime = 0.0f; //最大値を保持しておく変数
    bool  fadeInFlag = false;       //フェードイン用フラグ
    bool  fadeOutFlag = false;       //フェードアウト用フラグ


    DirectX::XMFLOAT2 originalTexSize = {};    //元のテクスチャサイズ

 
public:
    //変更する値を決めるenum
    enum ChangeValue {
        X_ONLY,
        Y_ONLY,
        X_AND_Y
    };
};
