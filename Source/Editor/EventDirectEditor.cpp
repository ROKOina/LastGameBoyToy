#include "EventDirectEditor.h"
#include "Component/Event/EventDirectCom.h"
#include <ImSequencer.h>
#include <Component\Event\EventDirect.h>
#include <Component\Event\EventMove.h>

EventDirectEditor::EventDirectEditor()
{
}

void EventDirectEditor::OnDraw(std::weak_ptr<GameObject> selectionGameObject)
{
#ifdef DEBUG
    if (selectionGameObject.lock() == nullptr)return;

    auto setEvent = selectionGameObject.lock()->GetComponent<EventDirectCom>();
    if (setEvent != nullptr)
    {
        if (!selectLockEnable)
        {
            eventDirectBehavior = selectionGameObject.lock()->GetComponent<EventDirectCom>();
            selectLockEnable = true;
        }
    }

    if (eventDirectBehavior.lock() == nullptr)return;
    auto eventDirect = eventDirectBehavior.lock()->GetEventData();
    if (eventDirect == nullptr)return;

    ImGui::Begin("AnimEvent", nullptr, ImGuiWindowFlags_None);

    float timer = eventDirectBehavior.lock()->GetTimer();

    if (debugUpdateEnable)
    {
        eventDirectBehavior.lock()->DebugEventUpdate();
        for (auto item : eventDirect->_EventItems)
        {
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(item.second))
            {
                inflexionEvent->_event.lock()->m_debugEnable = debugUpdateEnable;
            }
        }
    }
    if (ImGui::Button((char*)u8"保存"))
    {
        // 登録されているイベントアイテムを全てシリアライズさせる
        eventDirectBehavior.lock()->Serialize();
        for (auto item : eventDirect->_EventItems)
        {
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(item.second))
            {
                if (auto eventMove = std::dynamic_pointer_cast<EventMove>(inflexionEvent->_event.lock()->m_owner.lock()))
                {
                    eventMove->Serialize();
                }
            }
        }
    }
    ImGui::SameLine();

    ImGui::Checkbox("Lock", &selectLockEnable);

    ImGui::SameLine();

    ImGui::Checkbox(U("更新有無"), &debugUpdateEnable);
    ImGui::SameLine();

    // 名前の表示
    if (changeEnable)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
    }

    ImGui::SameLine();
    std::string eventName = eventDirectBehavior.lock()->GetGameObject()->GetName();
    ImGui::Text(((char*)u8"  イベントの名前 : " + eventName).c_str());

    if (changeEnable)
    {
        ImGui::SameLine();
        ImGui::Text("*");
        ImGui::PopStyleColor();
    }

    if (0 <= selectedEntry && selectedEntry < static_cast<int>(eventDirect->EventNames_.size()))
    {
        std::string tmp = eventDirect->EventNames_[selectedEntry];
        // イベント識別子の編集
        ImGui::SameLine();
        ImGui::Text(U("識別子"));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        auto currentEventIt = eventDirect->_EventItems.find(eventDirect->EventNames_[selectedEntry]);
        if (ImGui::InputString("##EventNameEditor", tmp, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            eventDirect->_EventItems.emplace(tmp, (*currentEventIt).second);
            eventDirect->EventNames_[selectedEntry] = tmp;
            eventDirect->_EventItems.erase(currentEventIt);

            currentEventIt = eventDirect->_EventItems.find(eventDirect->EventNames_[selectedEntry]);
        }
        ImGui::SameLine();

        //EventSystemValue e = EventSystemValue::NONE;

        ImGui::SetNextItemWidth(130);
        auto& value = (*currentEventIt).second->_Value;
        bool inputIntEnable = false;
        //// ゲームオブジェクトとコンポーネントの場合はアクティブ状態を選択するようにする
        //if (auto gameObject = std::dynamic_pointer_cast<EventGameObject>((*currentEventIt).second)) {

        //    bool activeEnable = value.i;
        //    ImGui::SameLine();
        //    if (ImGui::Checkbox("ActiveEnable", &activeEnable))
        //    {
        //        value.i = activeEnable;
        //    }
        //    inputIntEnable = true;
        //}

        //if (auto gameObject = std::dynamic_pointer_cast<EventComponent>((*currentEventIt).second)) {
        //    bool activeEnable = value.i;
        //    ImGui::SameLine();
        //    if (ImGui::Checkbox("ActiveEnable", &activeEnable))
        //    {
        //        value.i = activeEnable;
        //    }
        //    inputIntEnable = true;
        //}

        auto label = (char*)u8"付属値";
        switch ((*currentEventIt).second->_Type)
        {
        case VARIABLE_TYPE::INT:
            if (!inputIntEnable)
            {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(130);
                ImGui::InputInt(label, &value.i);
            }
            break;
        case VARIABLE_TYPE::LONG_LONG:
            ImGui::SameLine();
            ImGui::InputInt(label, &value.i);
            break;
        case VARIABLE_TYPE::FLOAT:
            ImGui::SameLine();
            ImGui::DragFloat(label, &value.f, 0.001f, 0.0f, 0.0f);
            break;
        case VARIABLE_TYPE::DOUBLE:
            ImGui::SameLine();
            ImGui::InputDouble(label, &value.d, 0.001);
            break;
        case VARIABLE_TYPE::FUNCTION:

            break;
        }

        if (ImGui::BeginPopupContextItem("Types"))
        {
            ImGui::ComboEnum("##EventValueTypes", (*currentEventIt).second->_Type);

            ImGui::EndPopup();
        }
    }

    // フレーム単位での処理にするかどうか
    bool unitFrameEbable = eventDirectBehavior.lock()->GetUnitFrameEbable();
    if (ImGui::Checkbox("Frame", &unitFrameEbable))
    {
        eventDirectBehavior.lock()->SetUnitFrameEbable(unitFrameEbable);
        if (unitFrameEbable)
        {
            eventDirect->_FlameLength *= 60;
            for (auto currentEventIt : eventDirect->_EventItems)
            {
                currentEventIt.second->_End *= 60;
                currentEventIt.second->_Start *= 60;
                if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(currentEventIt.second))
                {
                    // 時間設定
                    inflexionEvent->_event.lock()->SetDuration(currentEventIt.second->GetEventEndSec() - currentEventIt.second->GetEventStartSec());
                }
            }
        }
        else
        {
            eventDirect->_FlameLength /= 60;
            for (auto currentEventIt : eventDirect->_EventItems)
            {
                currentEventIt.second->_End /= 60;
                currentEventIt.second->_Start /= 60;
                if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(currentEventIt.second))
                {
                    // 時間設定
                    inflexionEvent->_event.lock()->SetDuration(currentEventIt.second->_End - currentEventIt.second->_Start);
                }
            }
        }
    }
    ImGui::SameLine();


    int lenInt = 0;
    if (unitFrameEbable)
    {
        lenInt = eventDirect->_FlameLength / 60;
        ImGui::SetNextItemWidth(30);
        ImGui::DragInt("SecLen", &eventDirect->_FlameLength);
        ImGui::SameLine();
    }
    else
    {
        lenInt = eventDirect->_FlameLength;
    }
    ImGui::SetNextItemWidth(30);
    if (ImGui::DragInt("Len", &lenInt))
    {
        if (unitFrameEbable)
        {
            eventDirect->_FlameLength = lenInt * 60;
        }
        else
        {
            eventDirect->_FlameLength = lenInt;
        }
    }
    ImGui::SameLine();

    // エディター内での再生をサポート
    std::string playButton[2]
    {
        "Play",
        "Stop"
    };
    std::string setPlayButtonName = (playEnable) ? playButton[1] : playButton[0];

    ImGui::SetNextItemWidth(50);
    if (ImGui::Button(setPlayButtonName.c_str()))
    {
        playEnable = !playEnable;
        for (auto item : eventDirect->_EventItems)
        {
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(item.second))
            {
                inflexionEvent->_event.lock()->m_decisionEnable = false;
            }
        }
    }
    ImGui::SameLine();
    if (playEnable)
    {
        TimeManager& timeManager = TimeManager::Instance();
        timer += 1.0f * timeManager.GetElapsedTime();
        float len = (unitFrameEbable) ? eventDirect->GetFlameLengthSec() : eventDirect->_FlameLength;
        if (timer < len)
            eventDirectBehavior.lock()->SetTimer(timer);
        else
        {
            eventDirectBehavior.lock()->SetTimer(0);
            playEnable = false;
        }
    }

    // タイムライン（旧バージョン）
    const Mouse& mouse{ Input::Instance().GetMouse() };

    float len = 0;
    if (unitFrameEbable)
    {
        len = (eventDirect->_FlameLength == 0) ? 0.0f : static_cast<float>(eventDirect->_FlameLength) / 60;
    }
    else
    {
        len = static_cast<float>(eventDirect->_FlameLength);
    }
    if (ImGui::SliderFloat("##Timeline", &timer, 0, len, "current frame = %.3f"))
    {
        if (!sequencerEbable)
        {
            eventDirectBehavior.lock()->SetTimer(timer);
            dragFloatEnable = true;
        }
    }
    else
    {
        if (Mouse::BTN_LEFT & mouse.GetButtonUp())
            dragFloatEnable = false;
    }

    // タイムラインシーケンサーの描画
    static int firstFrame = 0;
    static bool expanded = true;
    bool MovingCurrentFrame = false;
    int frame = (unitFrameEbable)? static_cast<int>(timer * 60) :static_cast<int>(timer);

    if (selectedEntry > eventDirect->_EventItems.size())selectedEntry = -1;
    ImSequencer::SEQUENCER_RET_TYPE retType;
    if (ImSequencer::Sequencer(eventDirect, &frame, &expanded, &selectedEntry, &firstFrame, MovingCurrentFrame,
        ImSequencer::SEQUENCER_EDIT_STARTEND |
        ImSequencer::SEQUENCER_ADD |
        ImSequencer::SEQUENCER_DEL |
        ImSequencer::SEQUENCER_COPYPASTE |
        ImSequencer::SEQUENCER_CHANGE_FRAME |
        ImSequencer::SEQUENCER_EDIT_ALL, retType))
    {
        changeEnable = true;
        for (auto item : eventDirect->_EventItems)
        {
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(item.second))
            {
                inflexionEvent->_event.lock()->m_decisionEnable = false;
            }
        }
        if (selectedEntry != -1)
        {
            auto currentEventIt = eventDirect->_EventItems.find(eventDirect->EventNames_[selectedEntry]);
            if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(currentEventIt->second))
            {
                // 時間設定
                if (unitFrameEbable)
                {
                    inflexionEvent->_event.lock()->SetDuration(currentEventIt->second->GetEventEndSec() - currentEventIt->second->GetEventStartSec());
                }
                else
                {
                    inflexionEvent->_event.lock()->SetDuration(currentEventIt->second->_End - currentEventIt->second->_Start);
                }
            }
            //if (mouse.Left.State == Button::Released)
            //{
            //    // 親オブジェクトを選択させるようにする
            //    if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(currentEventIt->second))
            //    {
            //        HIERARCHY_WINDOW->SetSelectGameObject(inflexionEvent->_event.lock()->GetOwner());
            //        inflexionEvent->_event.lock()->SetFocusComponent();
            //    }
            //}
        }
    }
    // 右クリックメニューでオブジェクトの作成・削除
    if (ImGui::BeginPopupContextItem("ContextMenu"))
    {
        if (ImGui::MenuItem((char*)u8"最後のオブジェクトに終端点を合わせる"))
        {
            int maxNum = std::numeric_limits<int>::min();
            for (auto item : eventDirect->_EventItems)
            {
                // 終端点を探す
                if (item.second->_End > maxNum)
                {
                    maxNum = item.second->_End;
                }
            }
            eventDirect->_FlameLength = maxNum + 1;
        }
        ImGui::EndPopup();
    }

    // シーケンサー内のシークバーを触った時の処理
    if (MovingCurrentFrame)
    {
        if (!dragFloatEnable)
        {
            eventDirectBehavior.lock()->SetTimer(frame);
            sequencerEbable = true;
        }
    }
    else
    {
        if (Mouse::BTN_LEFT & mouse.GetButtonUp())
            sequencerEbable = false;
    }

    // ドロップしたオブジェクトの更新処理
    DropUpdate(eventDirect);

    ImGui::End();
#endif // DEBUG
}

void EventDirectEditor::DropUpdate(std::shared_ptr<EventDirect>eventDirect)
{
#ifdef DEBUG
    if (ImGui::BeginDragDropTarget())
    {
        // イベントの登録
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EventMoveParameter"))
        {
            auto _event = *static_cast<std::shared_ptr<EventMoveParameterBehaviorBase>*>(payload->Data);
            if (_event)
            {
                bool singularEnable = false;

                if (auto moveParam = std::dynamic_pointer_cast<EventMoveParameterBehaviorBase>(_event))
                {
                    // 動きのイベント登録
                    if (!_event->m_eventUpdateEnable)
                    {
                        auto eventMoveParam = std::make_shared<EventMoveParameterBase>();
                        eventMoveParam->expanded = false;
                        eventMoveParam->_Start = 0;
                        eventMoveParam->_End = moveParam->m_duration;
                        eventMoveParam->_event = moveParam;
                        // 後からイベントを検索できるように
                        eventMoveParam->_gameObjName = moveParam->m_owner.lock()->GetGameObject()->GetName();
                        eventMoveParam->_componentRegisterId = moveParam->m_owner.lock()->GetRegisterId();
                        eventMoveParam->_eventRegisterId = moveParam->m_registerId;
                        // イベントで制御するように通知
                        moveParam->m_eventUpdateEnable = true;

                        eventDirect->AddEventMoveParameterBehaviorBaseEvent(std::move(eventMoveParam));
                        singularEnable = true;
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
#endif // DEBUG
}