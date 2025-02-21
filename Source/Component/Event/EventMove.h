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

    // イベントIDから取得
    std::shared_ptr<EventMoveParameterBehaviorBase> GetEventToRegisterId(int id)
    {
        int index = 0;
        std::shared_ptr<EventMoveParameterBehaviorBase> p;
        for (auto& eventMoveParameter : m_eventMoveParameters)
        {
            if (index == id)
            {
                p = eventMoveParameter;
                break;
            }
            index++;
        }
        return p;
    }

private:
    // GUI描画
    virtual void OnGUI();

    friend class EventDirectEditor;
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