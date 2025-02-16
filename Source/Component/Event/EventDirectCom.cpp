#include "EventDirectCom.h"
#include "EventMove.h"

EventDirectCom::EventDirectCom(const char* filename)
{
    if (filename)
    {
        this->filename = filename;
        std::ifstream istream(filename, std::ios::binary);
        if (istream.is_open())
        {
            cereal::BinaryInputArchive archive(istream);
            try
            {
                archive
                (
                    CEREAL_NVP(spc)
                );
            }
            catch (...)
            {
                LOG("event direct deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }

    if (spc.eventData == nullptr)
    {
        spc.eventData = std::make_shared<EventDirect>();
    }
}

void EventDirectCom::Start()
{
    // ゲームオブジェクトマネージャーからイベントに関するパラメータをリンクさせる
    for (auto& eventItem : spc.eventData->_EventItems)
    {
        if (auto& eventMoveParameter = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
        {
            auto eventObj = GameObjectManager::Instance().Find(eventMoveParameter->_gameObjName.c_str());
            auto eventMove = eventObj->GetComponentToRegisterId<EventMove>(eventMoveParameter->_componentRegisterId - 1);
            // パラメータリンク
            eventMoveParameter->_event = eventMove->GetEventToRegisterId(eventMoveParameter->_eventRegisterId -1);
        }
    }
    
    if (spc.initialPlaybackEbanle)
    {
        EventPlay();
    }
}

void EventDirectCom::Update(float elapsedTime)
{
    if (!spc.initialPlaybackEbanle)
    {
        if (!playEnable)return;
    }
    if (decisionEnable) return;// 動作が完了したら更新させない

    TimeManager& timeManager = TimeManager::Instance();
    if (!spc.reversePlayEnable)
    {
        if (spc.unscaleEnable) m_timer += 1.0f * timeManager.GetElapsedTime();
        else m_timer += 1.0f * timeManager.GetDeltaTime();
    }
    else
    {
        if (spc.unscaleEnable) m_timer -= 1.0f * timeManager.GetElapsedTime();
        else m_timer -= 1.0f * timeManager.GetDeltaTime();
    }

    EventUpdate();

    if (!spc.reversePlayEnable)
    {
        if (!spc.unitFrameEbable)
        {
            if (m_timer < spc.eventData->_FlameLength)return;
        }
        else
        {
            if (m_timer < spc.eventData->GetFlameLengthSec())return;
        }
    }
    else
    {
        if (m_timer >= 0)return;
    }
    decisionEnable = true;
    playEnable = false;
    for (auto eventItem : spc.eventData->_EventItems)
    {
        // 各コンポーネントに処理が終わったことを通知させる
        if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
        {
            inflexionEvent->_event.lock()->SetIsDecision(true);
        }
    }

    if (spc.loopEnable)
    {
        EventPlay();
    }

    if (spc.autoRemoveEnable)
    {
        //GetOwner()->Destroy(GetOwner());
    }
}

// イベント内の値を更新する処理
void EventDirectCom::EventUpdate()
    {
        if (!spc.unitFrameEbable)
        {
            int time = (int)m_timer;
            for (auto eventItem : spc.eventData->_EventItems)
            {
                // 動きの更新処理
                if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
                {
                    inflexionEvent->_event.lock()->SetTimer(m_timer - inflexionEvent->_Start);
                }
            }
        }
        else
        {
            for (auto eventItem : spc.eventData->_EventItems)
            {
                // 動きの更新処理
                if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
                {
                    inflexionEvent->_event.lock()->SetTimer(m_timer - inflexionEvent->GetEventStartSec());
                }
            }
        }
    }

// イベントの再生処理
void EventDirectCom::EventPlay()
    {
        m_timer = 0;
        decisionEnable = spc.reversePlayEnable = false;
        playEnable = true;
        for (auto eventItem : spc.eventData->_EventItems)
        {
            eventItem.second->_FunctionDecisionEnable = false;
            // 各コンポーネントに処理が終わったことを通知させる
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
            {
                inflexionEvent->_event.lock()->SetReverse(spc.reverseEnable);
                inflexionEvent->_event.lock()->SetIsDecision(false);
                inflexionEvent->_event.lock()->Start();
            }
        }
    }

// イベントの逆再生
void EventDirectCom::EventReversePlay()
    {
        spc.reversePlayEnable = true;
        playEnable = true;
        for (auto eventItem : spc.eventData->_EventItems)
        {
            eventItem.second->_FunctionDecisionEnable = false;
            // 各コンポーネントに処理が終わったことを通知させる
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
            {
                inflexionEvent->_event.lock()->SetReverse(spc.reverseEnable);
                inflexionEvent->_event.lock()->SetIsDecision(false);
            }
        }
    }

// イベントの停止
void EventDirectCom::EventStop()
    {
        playEnable = false;
    }

// イベントの停止からの再生
void EventDirectCom::EventResume()
    {
        playEnable = true;
        for (auto eventItem : spc.eventData->_EventItems)
        {
            eventItem.second->_FunctionDecisionEnable = false;
            // 各コンポーネントに処理が終わったことを通知させる
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
            {
                inflexionEvent->_event.lock()->SetReverse(spc.reverseEnable);
                inflexionEvent->_event.lock()->SetIsDecision(false);
            }
        }
    }

void EventDirectCom::EventSetStartValue()
    {
        for (auto eventItem : spc.eventData->_EventItems)
        {
            // 各コンポーネントにStartの値を設定する
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
            {
                inflexionEvent->_event.lock()->SetStartValue();
            }
        }
    }

void EventDirectCom::EventSetEndValue()
    {
        for (auto eventItem : spc.eventData->_EventItems)
        {
            // 各コンポーネントにEndの値を設定する
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
            {
                inflexionEvent->_event.lock()->SetEndValue();
            }
        }
    }

void EventDirectCom::EventSetFinish()
    {
        for (auto eventItem : spc.eventData->_EventItems)
        {
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
            {
                inflexionEvent->_event.lock()->SetIsDecision(true);
            }
        }
    }

// 演出イベントが現在再生中かを調べます
bool EventDirectCom::IsEventProgress(const std::string& eventKey) const
    {
        if (spc.eventData)
        {
            TimeManager& timeManager = TimeManager::Instance();
            auto it = spc.eventData->_EventItems.find(eventKey);
            if (!(it != spc.eventData->_EventItems.end()))
            {
                return false;
            }
            const std::shared_ptr<EventItemBase> event = it->second;
            if (!spc.unitFrameEbable)
            {
                if (event->_Start == event->_End)
                {
                    float next = (m_timer + timeManager.GetElapsedTime());
                    return !decisionEnable && m_timer <= event->_Start && event->_End < next;
                }
                else
                {
                    return event->_Start <= m_timer && m_timer <= event->_End;
                }
            }
            else
            {
                if (event->GetEventStartSec() + event->GetEventEndSec() == 0)
                {
                    return true;
                }
                if (event->GetEventStartSec() == event->GetEventEndSec())
                {
                    float next = (m_timer + timeManager.GetElapsedTime());
                    return !decisionEnable && m_timer <= event->GetEventStartSec() && event->GetEventEndSec() < next;
                }
                else
                {
                    return event->GetEventStartSec() <= m_timer && m_timer <= event->GetEventEndSec();
                }
            }
        }
        return false;
    }

// 指定したイベントデータを取得する
const Any* EventDirectCom::GetEventAdditionValue(const std::string& eventKey) const
    {
        if (spc.eventData)
        {
            auto it = spc.eventData->_EventItems.find(eventKey);
            if (it != spc.eventData->_EventItems.end())
            {
                return &it->second->_Value;
            }
        }
        return nullptr;
    }

void EventDirectCom::Serialize()
{
    if (this->filename != "")
    {
        std::ofstream ostream(filename, std::ios::binary);
        if (ostream.is_open())
        {
            cereal::BinaryOutputArchive archive(ostream);
            try
            {
                archive
                (
                    CEREAL_NVP(spc)
                );
            }
            catch (...)
            {
                LOG("event move deserialize failed.\n%s\n", filename);
                return;
            }
        }
        return;
    }

    static const char* filter = "EventMoveFiles(*.edm)\0*.edm;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "edm", Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        std::ofstream ostream(filename, std::ios::binary);
        if (ostream.is_open())
        {
            cereal::BinaryOutputArchive archive(ostream);

            try
            {
                this->filename = filename;
                archive
                (
                    CEREAL_NVP(spc)
                );
            }
            catch (...)
            {
                LOG("event direct deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

void EventDirectCom::OnGUI()
{
    if (ImGui::Button((char*)u8"保存"))
    {
        Serialize();
    }
    if (filename != "")
    {
        ImGui::SameLine();
        COPY_GUI(filename)
    }

    ImGui::Checkbox(U("生成と同時に再生"), &spc.initialPlaybackEbanle);
    ImGui::Checkbox(U("ループ再生"), &spc.loopEnable);
    ImGui::Checkbox(U("再生が終ったら自動削除"), &spc.autoRemoveEnable);
    ImGui::Checkbox(U("TimeScaleの影響を受けないか"), &spc.unscaleEnable);
    ImGui::Checkbox(U("関数の実行を必ず行うか"), &spc.surelyFunction);
    if (ImGui::Checkbox(U("再生を反転"), &spc.reverseEnable))
    {
        // リバース処理の適応
        for (auto eventItem : spc.eventData->_EventItems)
        {
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
            {
                inflexionEvent->_event.lock()->SetReverse(spc.reverseEnable);
            }
        }
    }
    if (ImGui::Button(U("再生")))
    {
        EventPlay();
    }
    ImGui::SameLine();
    if (ImGui::Button(U("逆再生")))
    {
        EventReversePlay();
    }
    ImGui::SameLine();
    if (ImGui::Button(U("停止")))
    {
        EventStop();
    }
    if (!playEnable)
    {
        ImGui::SameLine();
        if (ImGui::Button(U("続きから再生")))
        {
            EventResume();
        }
    }
}

#ifdef DEBUG
void EventDirectCom::DebugEventUpdate()
{
    if (!spc.unitFrameEbable)
    {
        int time = (int)m_timer;
        for (auto eventItem : spc.eventData->_EventItems)
        {
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
            {
                if (inflexionEvent->_event.lock())
                {
                    inflexionEvent->_event.lock()->SetTimer(m_timer - inflexionEvent->_Start);
                    inflexionEvent->_event.lock()->Update();
                }
                else
                {
                    inflexionEvent->SetEroorColor();
                    LOG("InflexionBaseが見つかりませんでした\n");
                }
            }
        }
    }
    else
    {
        for (auto eventItem : spc.eventData->_EventItems)
        {
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(eventItem.second))
            {
                if (inflexionEvent->_event.lock())
                {
                    inflexionEvent->_event.lock()->SetTimer(m_timer - inflexionEvent->GetEventStartSec());
                    inflexionEvent->_event.lock()->Update();
                }
                else
                {
                    inflexionEvent->SetEroorColor();
                    LOG("InflexionBaseが見つかりませんでした\n");
                }
            }
        }
    }
}
#endif

// シリアライズ
template<class Archive>
void EventDirectCom::SaveParameterCPU::serialize(Archive& archive, int version)
{
    if (version == 0)
    {
        archive
        (
            CEREAL_NVP(eventData),
            CEREAL_NVP(initialPlaybackEbanle),
            CEREAL_NVP(loopEnable),
            CEREAL_NVP(reverseEnable),
            CEREAL_NVP(unitFrameEbable),
            CEREAL_NVP(unscaleEnable),
            CEREAL_NVP(reversePlayEnable),
            CEREAL_NVP(autoRemoveEnable),
            CEREAL_NVP(surelyFunction)
        );
    }
}
CEREAL_CLASS_VERSION(EventDirectCom::SaveParameterCPU, 0)