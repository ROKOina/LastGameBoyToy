#include "EventMove.h"
#include <Input\Input.h>

EventMove::EventMove(const char* filename)
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
                    CEREAL_NVP(m_eventMoveParameters)
                );
            }
            catch (...)
            {
                LOG("event move deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

EventMove::~EventMove()
{
}

void EventMove::Update(float elapsedTime)
{
    for (auto eventMoveParameter : m_eventMoveParameters)
    {
        eventMoveParameter->Update();
    }
}

void EventMove::Start()
{
    for (auto eventMoveParameter : m_eventMoveParameters)
    {
        eventMoveParameter->SetOwner(shared_from_this());
        eventMoveParameter->Start();
    }
}

void EventMove::OnGUI()
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

    std::shared_ptr<EventMoveParameterBehaviorBase> deleteMoveParameter = nullptr;
    for (auto eventMoveParameter : m_eventMoveParameters)
    {
        ImGui::PushID(eventMoveParameter.get());
        if (ImGui::CollapsingHeader(eventMoveParameter->GetClassName_().c_str(), ImGuiTreeNodeFlags_DefaultOpen /*| ImGuiTreeNodeFlags_SpanFullWidth*/))
        {
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("EventMoveParameter", &(eventMoveParameter), sizeof(std::shared_ptr<EventMoveParameterBehaviorBase>));
                ImGui::Text((*eventMoveParameter).GetClassName_().c_str());
                ImGui::EndDragDropSource();
            }
            eventMoveParameter->OnGUI();
            ImGui::Spacing();
            if (ImGui::SmallButton("Delete"))
            {
                deleteMoveParameter = eventMoveParameter;
            }
        }
        ImGui::PopID();
    }

    if (deleteMoveParameter)
    {
        m_eventMoveParameters.remove(deleteMoveParameter);
    }

    ImGui::Spacing();

    // イベント一覧の表示
    bool isHovered = false;
    if (showEventMoveParameters)
    {
        ImVec2 size = { 400.0f, 500.0f };
        Mouse& mouse = Input::Instance().GetMouse();

        // マウスカーソル座標を取得
        ImVec2 pos = { static_cast<float>(mouse.GetPositionX()), static_cast<float>(mouse.GetPositionY()) };

        // 位置の補正を行う
        if (pos.x < 0) { pos.x = 0; }
        if (pos.y < 0) { pos.y = 0; }
        Graphics& Graphics = Graphics::Instance();
        if (pos.x + size.x > Graphics.GetScreenWidth()) { pos.x = Graphics.GetScreenWidth() - size.x; }
        if (pos.y + size.y > Graphics.GetScreenHeight()) { pos.y = Graphics.GetScreenHeight() - size.y; }

        // サイズは固定で移動は可能とし、フォーカスさせる
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
        ImGui::SetNextWindowFocus();
        // Window名は出さない
        ImGui::Begin("##AddEventWindow");
        // ウィンドウの当たり判定を取る
        isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
        // 出現直後なら検索欄を有効化
        if (ImGui::IsWindowAppearing())
        {
            ImGui::SetKeyboardFocusHere();
        }
        ImGui::InputText("##EventSearcher", searchName, 256);

        // 検索欄の入力が無ければすべて表示
        if (searchName[0] == '\0')
        {
            for (size_t i = 0; i < AddEventRegistry::GetClassName_().size(); ++i)
            {
                bool tmp = false;
                if (ImGui::Selectable(AddEventRegistry::GetClassName_()[i], &tmp))
                {
                    auto eventMoveParam = AddEventRegistry::GetEventMoveParameter()[i]->RegisterEvent(m_eventMoveParameters);
                    eventMoveParam->SetOwner(shared_from_this());
                    showEventMoveParameters = false;
                    break;
                }
            }
        }
        // 入力があれば一部を表示
        else
        {
            std::vector<const char*> itemNames;
            std::vector<size_t> itemSourceIndices; // std::functionのコピーが嫌なのでインデックスを保持
            // 上記の二つの配列に検索にかかった要素を挿入
            for (size_t i = 0; i < AddEventRegistry::GetClassName_().size(); ++i)
            {
                // 検索ワードを含むか調べる
                bool contain = LeftHasRight(AddEventRegistry::GetClassName_()[i], searchName);

                if (contain)
                {
                    itemSourceIndices.emplace_back(i);
                    itemNames.emplace_back(AddEventRegistry::GetClassName_()[i]);
                }
            }
            // 検索に引っ掛かったワードを羅列し、押されたら追加
            for (size_t i = 0; i < itemNames.size(); ++i)
            {
                bool tmp = false;
                if (ImGui::Selectable(itemNames[i], &tmp))
                {
                    auto eventMoveParam = AddEventRegistry::GetEventMoveParameter()[itemSourceIndices[i]]->RegisterEvent(m_eventMoveParameters);
                    eventMoveParam->SetOwner(shared_from_this());
                    showEventMoveParameters = false;
                    break;
                }
            }
        }
        ImGui::End();
    }

    if (ImGui::Button("Add Event"))
    {
        showEventMoveParameters = true;
    }
    else if (!isHovered && ImGui::IsMouseClicked(0))
    {
        showEventMoveParameters = false;
    }
}

// シリアライズ処理
void EventMove::Serialize()
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
                    CEREAL_NVP(m_eventMoveParameters)
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

    static const char* filter = "EventMoveFiles(*.evm)\0*.evm;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "evm", Graphics::Instance().GetHwnd());
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
                    CEREAL_NVP(m_eventMoveParameters)
                );
            }
            catch (...)
            {
                LOG("event move deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}
