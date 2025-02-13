#pragma once

namespace ImGui
{
    bool InputString(const char* label, ::std::string& text, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

    template <typename Enum>
    bool InputEnum(std::string labelName, Enum& selectProperty, int uniqueId = -1)
    {
        bool isReturn = false;
        const char* name = "";

        std::vector<std::string_view> propertyNames;
        for (const auto& enum_name : magic_enum::enum_names<Enum>())
        {
            propertyNames.emplace_back(enum_name);
        }

        std::string unique = "";
        if (uniqueId != -1)
        {
            unique = "##" + std::to_string(uniqueId);
        }

        std::string_view setName = magic_enum::enum_name(selectProperty);
        std::string dispName = labelName + setName.data() + unique;

        if (ImGui::TreeNode(dispName.c_str()))
        {
            size_t index = 0;
            for (const auto& propertyName : propertyNames)
            {
                ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;
                bool selected = false;
                if (setName == propertyName)
                {
                    nodeFlags |= ImGuiTreeNodeFlags_Selected;
                    selected = true;
                }

                ImGui::TreeNodeEx(propertyName.data(), nodeFlags, propertyName.data());

                // �N���b�N����ƑI��
                if (ImGui::IsItemClicked())
                {
                    selectProperty = magic_enum::enum_cast<Enum>(propertyName).value();
                    isReturn = true;
                }

                index++;

                ImGui::TreePop();
            }
            ImGui::TreePop();
            ImGui::Separator();
        }
        return isReturn;
    }

    template <typename Enum>
    bool InputEnum(std::string labelName, Enum& selectProperty, Enum max, int uniqueId = -1)
    {
        bool isReturn = false;
        const char* name = "";

        std::vector<std::string_view> propertyNames;
        for (const auto& enum_name : magic_enum::enum_names<Enum>())
        {
            if (enum_name == magic_enum::enum_name(max))
                break;
            propertyNames.emplace_back(enum_name);
        }

        std::string unique = "";
        if (uniqueId != -1)
        {
            unique = "##" + std::to_string(uniqueId);
        }

        std::string_view setName = magic_enum::enum_name(selectProperty);
        std::string dispName = labelName + setName.data() + unique;

        if (ImGui::TreeNode(dispName.c_str()))
        {
            size_t index = 0;
            for (const auto& propertyName : propertyNames)
            {
                ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;
                bool selected = false;
                if (setName == propertyName)
                {
                    nodeFlags |= ImGuiTreeNodeFlags_Selected;
                    selected = true;
                }

                ImGui::TreeNodeEx(propertyName.data(), nodeFlags, propertyName.data());

                // �N���b�N����ƑI��
                if (ImGui::IsItemClicked())
                {
                    selectProperty = magic_enum::enum_cast<Enum>(propertyName).value();
                    isReturn = true;
                }

                index++;

                ImGui::TreePop();
            }
            ImGui::TreePop();
            ImGui::Separator();
        }
        return isReturn;
    }

    template <typename Enum>
    bool ComboEnum(std::string labelName, Enum& selectProperty)
    {
        bool isReturn = false;

        // 列挙型の名前を取得し、文字列を結合して\0区切りの形式にする
        static std::string combo_items;
        static std::vector<std::string> enum_names;
        if (enum_names.empty()) {
            for (auto name : magic_enum::enum_names<Enum>()) {
                enum_names.emplace_back(name);
                combo_items += name;
                combo_items += '\0'; // ImGui::Comboで必要なフォーマット
            }
        }

        int current_index = static_cast<int>(selectProperty);

        // ImGui::Comboの描画
        if (ImGui::Combo(labelName.c_str(), &current_index, combo_items.c_str()))
        {
            selectProperty = static_cast<Enum>(current_index);
            isReturn = true;
        }
        return isReturn;
    }

    template <class Value>
    inline bool EditContainerElement(Value& value) { return false; }

    template <>
    bool EditContainerElement(int& value);

    template <>
    bool EditContainerElement(UINT& value);

    template <>
    bool EditContainerElement(float& value);

    template <class Container, class ...Args>
    bool InputContainer(const std::string& label, Container& container, Args... args)
    {
        bool result = false;
        if (ImGui::CollapsingHeader(label.c_str()))
        {
            int i = 0;
            auto it = container.begin();
            while (it != container.end())
            {
                std::string treeLabel = "Element : " + std::to_string(i);
                if (ImGui::TreeNode(treeLabel.c_str()))
                {
                    result |= ImGui::EditContainerElement(*it, args...);
                    if (ImGui::Button("Delete"))
                    {
                        it = container.erase(it);
                        result = true;
                        continue;
                    }
                    ImGui::TreePop();
                }

                ++it;
                ++i;
            }
            if (ImGui::Button("Add"))
            {
                container.insert(container.end(), Container::value_type());
                result = true;
            }
        }
        return result;
    }
}