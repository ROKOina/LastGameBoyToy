#pragma once
#include "EventMoveBase.h"

// イベントのパラメータにアクセスするためのコンポーネント
class EventMove : public Component, public std::enable_shared_from_this<EventMove>
{
public:
    EventMove(const char* filename);
    ~EventMove();

    //名前設定
    const char* GetName() const override { return "EventMove"; }

public:
    virtual void Start() override;
    virtual void Update(float elapsedTime);

private:
    // GUI描画
    virtual void OnGUI();

    //シリアライズ
    void Serialize();

private:
    std::list<std::shared_ptr<EventMoveParameterBehaviorBase>> m_eventMoveParameters;

    // デバッグで使用するパラメータ
    bool showEventMoveParameters = false;
    char searchName[256] = {};
    std::string filename = "";
    COPY_VARIABLE()
};