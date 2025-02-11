#pragma once
#include "EventMoveBase.h"

// イベントのパラメータにアクセスするためのコンポーネント
class EventMove : public Component, public std::enable_shared_from_this<EventMove>
{
public:
    // TODO:シリアライズの処理を追加
    EventMove();
    ~EventMove();

    //名前設定
    const char* GetName() const override { return "EventMove"; }

public:
    virtual void Start() override;
    virtual void Update(float elapsedTime);

private:
    // GUI描画
    virtual void OnGUI();

private:
    std::vector<std::shared_ptr<EventMoveParameterBehaviorBase>> m_eventMoveParameters;

    // デバッグで使用するパラメータ
    bool showEventMoveParameters = false;
    char searchName[256] = {};
};