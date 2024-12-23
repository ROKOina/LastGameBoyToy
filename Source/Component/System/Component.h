#pragma once

#include "GameObject.h"

//コンポーネント
class Component
{
public:
    Component() {}
    virtual ~Component() {}

    // 名前取得
    virtual const char* GetName() const = 0;

    // 開始処理
    virtual void Start() {}

    // 更新処理
    virtual void Update(float elapsedTime) {}

    // 破棄処理
    virtual void OnDestroy() {}

    // GUI描画
    virtual void OnGUI() {}

    //音解放
    virtual void AudioRelease() {}

    // ゲームオブジェクト設定
    void SetGameObject(std::shared_ptr<GameObject> obj) { this->gameObject_ = obj; }

    // ゲームオブジェクト取得
    std::shared_ptr<GameObject> GetGameObject() { return gameObject_.lock(); }

    const bool GetEnabled() const { return isEnabled_; }
    void SetEnabled(bool enabled) { isEnabled_ = enabled; }

    const float GetLoadTime() const { return loadTime; }
    void SetLoadTime(float time) { loadTime = time; }

private:
    std::weak_ptr<GameObject>	gameObject_;
    float loadTime = 0;

protected:
    //有効か
    bool isEnabled_ = true;
};