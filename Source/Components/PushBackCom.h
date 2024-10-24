#pragma once

#include "System\Component.h"

//押し返しするコンポーネント

//PushBackCom
class PushBackCom : public Component
{
    //コンポーネントオーバーライド
public:
    PushBackCom();
    ~PushBackCom() {}

    // 名前取得
    const char* GetName() const override { return "PushBackCom"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    // debug描画
    void DebugRender();

    void ResetHitFlg() { isHit = false; }

    void PushBackUpdate(std::shared_ptr<PushBackCom> otherSide);

    //重さ設定
    void SetWeight(float weight) { weight_ = weight; }
    float GetWeight() { return weight_; }
    //半径設定
    void SetRadius(float radius) { radius_ = radius; }
    float GetRadius() { return radius_; }
private:
    bool isHit = false;

    float radius_ = 0.5f;

    //重さ
    float weight_ = 1;
};