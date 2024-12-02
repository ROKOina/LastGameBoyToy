#pragma once

#include "Component/System/Component.h"

//ネットに送る用
class HitProcessCom : public Component
{
public:
    //myObjにキャラクターコンポーネントを入れる
    HitProcessCom(std::shared_ptr<GameObject> charaObj)
        : hitIntervalTimer(0), myObj(charaObj)
    {}
    ~HitProcessCom() override {}

    // 名前取得
    const char* GetName() const override { return "HitProcess"; }

    // 開始処理
    void Start() override {}

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    enum class HIT_TYPE
    {
        DAMAGE,
        HEAL,
        STAN,
        KNOCKBACK,
    };
    void SetHitType(HitProcessCom::HIT_TYPE type) { hitType = type; }

    void SetHitInterval(float sec) { hitIntervalTime = sec; }

    void SetValue(float value) { this->value = value; }
    void SetValue3(DirectX::XMFLOAT3 value3) { this->value3 = value3; }

    bool IsHit() { return isHit; }
    bool IsHitNonChara(std::shared_ptr<GameObject>& hitObj) {
        hitObj = nonCharaObj.lock();
        return isHitNonChara;
    }

private:
    void HitProcess(int myID, int hitID);

private:
    //弾の持ち主
    std::weak_ptr<GameObject> myObj;

    //種類
    HIT_TYPE hitType = HIT_TYPE::DAMAGE;

    //ヒット間隔(レイキャスト用)
    float hitIntervalTime = 0.1f;
    float hitIntervalTimer;

    //与える数
    float value = 1.0f; //DAMAGE, HEAL, STAN
    DirectX::XMFLOAT3 value3 = { 0,0,0 };   //KNOCKBACK

    //当たっているか
    bool isHit = false;

    //キャラがない敵に当たった
    bool isHitNonChara = false;
    std::weak_ptr<GameObject> nonCharaObj;  //オブジェクトを保存
};