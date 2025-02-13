#pragma once

class GameObject;
//コンポーネント
class Component
{
    friend class GameObject;
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

    // ゲームオブジェクト取得
    std::shared_ptr<GameObject> GetGameObject() { return gameObject_.lock(); }

    const bool GetEnabled() const { return isEnabled_; }
    void SetEnabled(bool enabled) { isEnabled_ = enabled; }

    const float GetLoadTime() const { return loadTime; }
    void SetLoadTime(float time) { loadTime = time; }

    const int GetRegisterId() const { return registerId; }

private:
    std::weak_ptr<GameObject>	gameObject_;
    float loadTime = 0;
    int registerId = -1;

protected:
    //有効か
    bool isEnabled_ = true;
};