#pragma once

#include "Components/System/Component.h"

class EventCom :public Component
{
public:

    EventCom(const char* filename);
    ~EventCom() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Event"; }

public:

    // イベントのパラメータを格納する構造体
    struct EventParameter
    {
        float starttime = 0.0f;        // 開始時間
        float totaltime = 0.0f;        // 経過時間
        float endtime = 0.0f;          // 終了時間
        std::string eventname = "New Event";
        bool active = false;           // イベントの有効/無効状態
        bool finished = false;         // イベントの完了状態
        bool loop = false;             // ループフラグ
    };
    std::vector<EventParameter> eventList;
};