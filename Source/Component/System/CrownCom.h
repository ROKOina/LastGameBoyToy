#pragma once

class GameObject;

//ゲームモード王冠のオブジェクト
class CrownCom : public Component
{
public:
    //myObjにキャラクターコンポーネントを入れる
    CrownCom()
    {
    }
    ~CrownCom() override {}

    // 名前取得
    const char* GetName() const override { return "Crown"; }

    // 開始処理
    void Start() override {}

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

public:
    bool HaveCrown();
    //クラウンを手放した時のポス
    DirectX::XMFLOAT3 GetLastPos();

    //オンライン王冠ゲット
    void GetCrown(std::shared_ptr<GameObject> obj);
    void DelCrown();    //手放す

    //王冠誰か持っているか
    bool RegisteredCrown();

    float GetHaveTimer() { return haveTimer; }

private:
    void IsCrownHaveUpdate(float elapsedTime);  //王冠を所持している時
    void IsCrownNonHaveUpdate(float elapsedTime);   //王冠を所持していない時

private:
    bool haveCrownObject = false;   //所持中か見極めるため
    float haveTimer = 0;    //所持時間
    std::weak_ptr<GameObject> haveObj;  //所持者
    DirectX::XMFLOAT3 lastPos = {};
    float secondGetTimer = 0;   //死亡時取らないように

    //リスポーンコンポーネントに合わせる
    float playerDeathHeight = -50;  //クラウン落ちたとき用
};