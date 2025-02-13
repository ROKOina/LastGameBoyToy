
namespace ImGui
{
    bool InputString(const char* label, ::std::string& text, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
    {
        size_t clipboardBufSize = 0;
        if (OpenClipboard(nullptr)) {
            // get clip board data
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData != nullptr) {
                // get buf size
                clipboardBufSize = GlobalSize(hData);
            }

            CloseClipboard();
        }

        // constexpr size_t newCharBufferSpace = 10;
        static int newCharBufferSpace = 100;

        size_t bufSize = text.capacity() + clipboardBufSize + newCharBufferSpace;
        char* buf = new char[bufSize];

        ZeroMemory(buf, bufSize);
        memcpy_s(buf, bufSize, text.data(), text.size());

        bool r = ImGui::InputText(label, buf, bufSize, flags, callback, user_data);

        if (r)
        {
            text = buf;
        }

        delete[] buf;

        return r;
    }

    template <>
    bool EditContainerElement<UINT>(UINT& value)
    {
        int signedValue = static_cast<int>(value);
        bool result = ImGui::DragInt("UINT", &signedValue, 0.1f, 0, INT_MAX);
        value = static_cast<UINT>(signedValue);

        return result;
    }

    template<>
    bool EditContainerElement(float& value)
    {
        return ImGui::DragFloat("FLOAT", &value);
    }

    template<>
    bool EditContainerElement(int& value)
    {
        return ImGui::DragInt("INT", &value, 0.1f);
    }
}