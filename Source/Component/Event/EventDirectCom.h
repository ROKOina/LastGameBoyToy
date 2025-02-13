#pragma once
#include "EventDirect.h"

// 全てのイベントの最後に生成するようにして下さい
class EventDirectCom : public Component
{
public:
    EventDirectCom(const char* filename);
    ~EventDirectCom() {};

public:
    virtual void Start() override;
    virtual void Update(float elapsedTime) override;

private:
    // イベント内の値を更新する処理
    void EventUpdate();

    // イベントのシリアライズ処理
    void Serialize();

#ifdef DEBUG
    void DebugEventUpdate();

    // タイマーのセット
    void SetTimer(float timer) { this->m_timer = timer; }

    // 単位をフレームにするかどうかの設定
    void SetUnitFrameEbable(bool unitFrameEbable) { spc.unitFrameEbable = unitFrameEbable; }
#endif

public:
    //! @brief  イベントの再生
    void EventPlay();

    //! @brief  イベントの逆再生
    void EventReversePlay();

    //! @brief  イベントの停止
    void EventStop();

    //! @brief  イベントの停止からの再生
    void EventResume();

    //! @brief  動きのコンポーネントの値をStartの値にする
    void EventSetStartValue();

    //! @brief  動きのコンポーネントの値をEndの値にする
    void EventSetEndValue();

    //! @brief  動きのコンポーネントの更新状態を終了状態にする
    void EventSetFinish();

    //! @brief  動きのコンポーネントの反転状態の可否
    void SetReverseEnable(bool reverseEnable) { spc.reverseEnable = reverseEnable; }

    //! @brief  動きのコンポーネントの反転状態の可否取得
    bool GetReverseEnable() { return spc.reverseEnable; }

    //! @brief  再生中かどうか取得
    bool GetPlayEnable() { return playEnable; }

    //! @brief  再生が終わったかどうか取得
    bool GetFinishEnable() { return decisionEnable; }

    //! @brief  演出イベントが現在再生中かを調べます
    bool IsEventProgress(const std::string& eventKey) const;

    //! @brief  演出イベントに紐づけられた値を取得します
    const Any* GetEventAdditionValue(const std::string& eventKey) const;

    //! @brief  イベントの長さを取得します
    int GetEventLength() { return spc.eventData->_FlameLength; }

    //! @brief  イベントデータ取得
    std::shared_ptr<EventDirect> GetEventData() { return spc.eventData; }

    //! @brief  タイマーの取得
    float GetTimer() { return m_timer; }

    //! @brief  単位をフレームにするかどうか取得
    bool GetUnitFrameEbable() { return spc.unitFrameEbable; }
private:
    // GUI描画
    virtual void OnGUI();

public:
    // 保存パラメータ
    struct SaveParameterCPU
    {
        std::shared_ptr<EventDirect> eventData;
        // 最初に自動で再生させるか
        bool initialPlaybackEbanle = true;
        bool loopEnable = false;
        bool reverseEnable = false;
        // 単位をフレームにするかどうか
        bool unitFrameEbable = false;
        bool unscaleEnable = false;
        bool reversePlayEnable = false;
        // 再生終了時に自動で削除させるか
        bool autoRemoveEnable = false;
        // フレーム落ちした場合でも必ず関数を実行させるか
        bool surelyFunction = true;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
private:
    SaveParameterCPU spc = {};
    float m_timer = 0;
    bool decisionEnable = false;
    bool playEnable = false;

    // デバッグ時に使用
    std::string filename = "";
    COPY_VARIABLE()
};

